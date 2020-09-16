#include "PMonteCarloPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <string>

PMonteCarloPlayer::PMonteCarloPlayer(int rollouts, bool verbose)
{
    this->rollouts = rollouts;
    this->verbose = verbose;
}

Engine::IMove *PMonteCarloPlayer::choose_move(Engine::IBoard *board)
{

    int my_turn = board->turn % 2;

    int max_depth_reached = 0;

    int iterations = 0;

    omp_lock_t debug;
    omp_init_lock(&debug);

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, PScore> scores;

    // Creates root node for the game-tree starting from the current board configuration.
    PNode root = PNode(board->get_copy());

    omp_set_num_threads(3);

    #pragma omp parallel for reduction(max:max_depth_reached)
    for(int iterations = 0; iterations < rollouts; iterations++)
    {
        PNode *node = &root;

        // 1) Tree traversing
        // Search for the most interesting node to be tested.
        // Starting from the root node, select the children node with the best ucb value
        // Repeat until we found a node not yet expanded

        int depth = 0;

        omp_set_lock(&debug);
        std::cout << omp_get_thread_num() << " iteration " << iterations << "\n";
        std::cout.flush();
        omp_unset_lock(&debug);

        // Keep searching for a node not yet expanded
        while (node->is_expanded())
        {

            PNode *selected_node;
            float highscore = std::numeric_limits<int>::min();
            float score;

            for (int i = 0; i < node->possible_moves->size(); i++)
            {
                PNode *n = node->children[i];

                // Check if node is being tested
                if(!n->is_under_rollout())
                {                 
                    if (node->board->turn % 2 == my_turn)
                    {
                        // If it's my turn, use the ucb value that maximize my winrate
                        score = scores[n->id].get_ucb(scores[root.id].get_played());
                    }
                    else
                    {
                        // If it's my opponent's turn, use the ucb value that minimize my winrate
                        score = scores[n->id].get_inverse_ucb(scores[root.id].get_played());
                    }
                }
                else
                {
                    // "Virtual loss"
                    score = -1;
                }
                
                if (score > highscore)
                {
                    selected_node = n;
                    highscore = score;
                }
            }

            depth++;

            node = selected_node;

            // omp_set_lock(&debug);
            // std::cout << omp_get_thread_num() << " traversing down node " << node->id << " highscore " << highscore << std::endl;
            // std::cout.flush();
            // omp_unset_lock(&debug);
        }

        if (depth > max_depth_reached)
            max_depth_reached = depth;
        
        // 2) Expansion
        // If this node it's not a terminal node, expand it

        float gain = 0;
        int played = 1;

        omp_set_lock(&debug);
        std::cout << omp_get_thread_num() << " rollout " << node->id << "\n";
        std::cout.flush();
        omp_unset_lock(&debug);

        PNode *rollout_node = node;

        rollout_node->set_under_rollout(true);

        if (!node->is_leaf())
        {
            // Expand node
            node->expand();

            // Pick one children node
            node = node->children[0];

            // 3) Rollout
            // Simulate a random game from the node's board configuration

            Engine::IBoard *test_board = node->board->get_copy();
            Engine::play(test_board, &player, &player);

            // The board is now in a terminal state, get the outcome
            if (test_board->status == Engine::IBoard::Draw)
            {
                gain = 0.5f;
            }
            else if ((my_turn == 1 && test_board->status == Engine::IBoard::First) || (my_turn == 0 && test_board->status == Engine::IBoard::Second))
            {
                gain = 1;
            }

            delete test_board;
        }
        else
        {
            // Node it's a terminal node, get the outcome
            if (node->board->status == Engine::IBoard::Draw)
            {
                gain = 0.5f;
            }
            else if ((my_turn == 1 && node->board->status == Engine::IBoard::First) || (my_turn == 0 && node->board->status == Engine::IBoard::Second))
            {
                gain = 1;
            }
        }

        rollout_node->set_under_rollout(false);
        
        omp_set_lock(&debug);
        std::cout << omp_get_thread_num() << " rollout ended " << node->id << "\n";
        std::cout.flush();
        omp_unset_lock(&debug);

        // 4) Backpropagation
        // Backpropagate the outcome to parent node until we reach the root node
       
        while (node != NULL)
        {
            // Update node's values
            scores[node->id].increase(gain, played);

            // Go to parent
            node = node->parent;

            // if(node != NULL)
            // {    
            //     omp_set_lock(&debug);
            //     std::cout << omp_get_thread_num() << " traversing up node " << node->id << "\n";
            //     std::cout.flush();
            //     omp_unset_lock(&debug);
            // }
        }
    }

    // Now we can select the move with the highest winrate
    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Current win rate: " << (scores[root.id].get_winrate() * 100) << "%" << std::endl;
    }

    PNode *selected_node;
    float highscore = -1;

    // Select the node children of root with the best winrate
    for (int i = 0; i < root.possible_moves->size(); i++)
    {
        PNode *n = root.children[i];
        float ucb = scores[n->id].get_ucb(scores[root.id].get_played());
        float winrate = scores[n->id].get_winrate();
        if (winrate > highscore)
        {
            selected_node = n;
            highscore = winrate;
        }

        if (verbose)
            std::cout << n->move->to_string() << ": " << (winrate * 100) << "% played: " << scores[n->id].get_played() << " ucb: " << ucb << std::endl;
    }

    Engine::IMove *selected_move = selected_node->move->get_copy();

    return selected_move;
}
