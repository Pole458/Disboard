#include "PTMonteCarloPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <string>
#include <chrono>

PTMonteCarloPlayer::PTMonteCarloPlayer(int rollouts, bool verbose)
{
    this->rollouts = rollouts;
    this->verbose = verbose;
}

Engine::IMove *PTMonteCarloPlayer::choose_move(Engine::IBoard *board)
{

    int my_turn = board->turn % 2;

    int max_depth_reached = 0;

    omp_lock_t debug;
    omp_init_lock(&debug);

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, PScore> scores;

    omp_lock_t scores_lock;
    omp_init_lock(&scores_lock);

    // Creates root node for the game-tree starting from the current board configuration.
    PNode root = PNode(board->get_copy());

    scores.try_emplace(root.id);

    omp_set_num_threads(2);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#pragma omp parallel for reduction(max \
                                   : max_depth_reached)
    for (int iterations = 0; iterations < rollouts; iterations++)
    {
        PNode *node = &root;

        // 1) Tree traversing
        // Search for the most interesting node to be tested.
        // Starting from the root node, select the children node with the best ucb value
        // Repeat until we found a node not yet expanded

        // omp_set_lock(&debug);
        // std::cout << omp_get_thread_num() << " iteration " << iterations << std::endl;
        // std::cout.flush();
        // omp_unset_lock(&debug);

        int depth = 0;

        // Keep searching for a node not yet expanded
        while (node->is_expanded())
        {

            PNode *selected_node;
            float highscore = std::numeric_limits<int>::min();
            float score;

            for (int i = 0; i < node->possible_moves->size(); i++)
            {
                PNode *n = node->children[i];

                // Create score for node if it doesn't exists
                if (scores.find(n->id) == scores.end())
                {
                    omp_set_lock(&scores_lock);
                    scores.try_emplace(n->id);
                    omp_unset_lock(&scores_lock);
                }

                omp_set_lock(&scores_lock);

                if (node->board->turn % 2 == my_turn)
                {
                    // If it's my turn, use the ucb value that maximize my winrate
                    score = scores.at(n->id).get_ucb(scores.at(root.id).get_played());
                }
                else
                {
                    // If it's my opponent's turn, use the ucb value that minimize my winrate
                    score = scores.at(n->id).get_inverse_ucb(scores.at(root.id).get_played());
                }

                omp_unset_lock(&scores_lock);

                if (score > highscore)
                {
                    selected_node = n;
                    highscore = score;
                }
            }

            depth++;

            node = selected_node;
        }

        if (depth > max_depth_reached)
            max_depth_reached = depth;

        // 2) Expansion
        // If this node it's not a terminal node, expand it

        float gain = 0;
        int played = 1;

        if (!node->is_leaf())
        {
            // Expand node
            node->expand();

            // Pick one children node
            node = node->children[0];

            // 3) Rollout
            // Simulate a random game from the node's board configuration

            // Create score for node if it doesn't exists
            if (scores.find(node->id) == scores.end())
            {
                omp_set_lock(&scores_lock);
                scores.try_emplace(node->id);
                omp_unset_lock(&scores_lock);
            }

            // Virtual loss

            omp_set_lock(&scores_lock);
            scores.at(node->id).set_virtual_loss(true);
            omp_unset_lock(&scores_lock);

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
            scores.at(node->id).set_virtual_loss(false);
            omp_unset_lock(&scores_lock);
        }
        else
        {

            // Virtual loss
            omp_set_lock(&scores_lock);
            scores.at(node->id).set_virtual_loss(true);
            omp_unset_lock(&scores_lock);

            // Node it's a terminal node, get the outcome
            if (node->board->status == Engine::IBoard::Draw)
            {
                gain = 0.5f;
            }
            else if ((my_turn == 1 && node->board->status == Engine::IBoard::First) || (my_turn == 0 && node->board->status == Engine::IBoard::Second))
            {
                gain = 1;
            }

            omp_set_lock(&scores_lock);
            scores.at(node->id).set_virtual_loss(false);
            omp_unset_lock(&scores_lock);
        }

        // 4) Backpropagation
        // Backpropagate the outcome to parent node until we reach the root node

        while (node != NULL)
        {
            // Update node's values
            omp_set_lock(&scores_lock);
            scores.at(node->id).increase(gain, played);
            omp_unset_lock(&scores_lock);

            // Go to parent
            node = node->parent;
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl
                  << "Played: " << scores[root.id].get_played() << std::endl
                  << "Current win rate: " << (scores[root.id].get_winrate() * 100) << "%" << std::endl;
    }

    // Now we can select the move with the highest winrate

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
