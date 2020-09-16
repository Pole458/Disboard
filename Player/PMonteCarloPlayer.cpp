#include "PMonteCarloPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <string>
#include <chrono>

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

    omp_lock_t scores_lock;
    omp_init_lock(&scores_lock);

    // Creates root node for the game-tree starting from the current board configuration.
    PNode root = PNode(board->get_copy());
    
    // omp_set_num_threads(2);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    #pragma omp parallel for reduction(max:max_depth_reached)
    for(int iterations = 0; iterations < rollouts; iterations++)
    {
        PNode *node = &root;

        // 1) Tree traversing
        // Search for the most interesting node to be tested.
        // Starting from the root node, select the children node with the best ucb value
        // Repeat until we found a node not yet expanded

        int depth = 0;

        // omp_set_lock(&debug);
        // std::cout << omp_get_thread_num() << " request 0 " << node->id << std::endl;
        // std::cout.flush();
        // omp_unset_lock(&debug);

        omp_set_lock(&node->lock);

        // omp_set_lock(&debug);
        // std::cout << omp_get_thread_num() << "     set 0 " << node->id << std::endl;
        // std::cout.flush();
        // omp_unset_lock(&debug);

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
                if(omp_test_lock(&n->lock))
                {                 
                    if (node->board->turn % 2 == my_turn)
                    {
                        // If it's my turn, use the ucb value that maximize my winrate
                        omp_set_lock(&scores_lock);
                        score = scores[n->id].get_ucb(scores[root.id].get_played());
                        omp_unset_lock(&scores_lock);
                    }
                    else
                    {
                        omp_set_lock(&scores_lock);
                        // If it's my opponent's turn, use the ucb value that minimize my winrate
                        score = scores[n->id].get_inverse_ucb(scores[root.id].get_played());
                        omp_unset_lock(&scores_lock);
                    }

                    omp_unset_lock(&n->lock);
                }
                else
                {
                    // "Virtual loss"
                    // omp_set_lock(&debug);
                    // std::cout << omp_get_thread_num() << " virtual loss for " << n->id << std::endl;
                    // std::cout.flush();
                    // omp_unset_lock(&debug);
                    score = -1;
                }
                
                if (score > highscore)
                {
                    selected_node = n;
                    highscore = score;
                }
            }

            depth++;

            // omp_set_lock(&debug);
            // std::cout << omp_get_thread_num() << " request 1 " << selected_node->id << std::endl;
            // std::cout.flush();
            // omp_unset_lock(&debug);

            omp_set_lock(&selected_node->lock);

            // omp_set_lock(&debug);
            // std::cout << omp_get_thread_num() << "    set 1 " << selected_node->id << std::endl;
            // std::cout.flush();
            // omp_unset_lock(&debug);

            omp_unset_lock(&node->lock);

            // omp_set_lock(&debug);
            // std::cout << omp_get_thread_num() << "   unset 1 " << node->id << std::endl;
            // std::cout.flush();
            // omp_unset_lock(&debug);

            node = selected_node;
          
        }

        if (depth > max_depth_reached)
            max_depth_reached = depth;
        
        // 2) Expansion
        // If this node it's not a terminal node, expand it

        float gain = 0;
        int played = 1;

        // omp_set_lock(&debug);
        // std::cout << omp_get_thread_num() << " rollout " << node->id << "\n";
        // std::cout.flush();
        // omp_unset_lock(&debug);

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

            omp_set_lock(&scores_lock);
            scores[node->id].increase(gain, played);
            omp_unset_lock(&scores_lock);

            node = node->parent;
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

        // 4) Backpropagation
        // Backpropagate the outcome to parent node until we reach the root node
      
        omp_unset_lock(&node->lock);

        // omp_set_lock(&debug);
        // std::cout << omp_get_thread_num() << "   unset 4 " << node->id << std::endl;
        // std::cout.flush();
        // omp_unset_lock(&debug);

        while (node != NULL)
        {
            // Update node's values
            omp_set_lock(&scores_lock);
            scores[node->id].increase(gain, played);
            omp_unset_lock(&scores_lock);

            // Go to parent
            node = node->parent;
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // Now we can select the move with the highest winrate
    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Current win rate: " << (scores[root.id].get_winrate() * 100) << "%" << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl;
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
