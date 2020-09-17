#include "PRMonteCarloPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <string>
#include <chrono>

PRMonteCarloPlayer::PRMonteCarloPlayer(int rollouts, bool verbose)
{
    this->rollouts = rollouts;
    this->verbose = verbose;
}

Engine::IMove *PRMonteCarloPlayer::choose_move(Engine::IBoard *board)
{

    int my_turn = board->turn % 2;

    int max_depth_reached = 0;

    int iterations = 0;

    omp_lock_t debug;
    omp_init_lock(&debug);

    // Creates root node for the game-tree starting from the current board configuration.
    Node global_root = Node(board->get_copy());
    global_root.expand();

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, Score> global_scores;

    // omp_set_num_threads(2);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#pragma omp parallel reduction(max \
                               : max_depth_reached)
    {

        Node root = Node(board->get_copy());

        std::unordered_map<Engine::board_id, Score> scores;

        for (int iterations = 0; iterations < rollouts; iterations++)
        {
            Node *node = &root;

            // 1) Tree traversing
            // Search for the most interesting node to be tested.
            // Starting from the root node, select the children node with the best ucb value
            // Repeat until we found a node not yet expanded

            int depth = 0;

            // Keep searching for a node not yet expanded
            while (node->expanded)
            {

                Node *selected_node;
                float highscore = std::numeric_limits<int>::min();
                float score;

                for (int i = 0; i < node->possible_moves->size(); i++)
                {
                    Node *n = node->children[i];

                    if (node->board->turn % 2 == my_turn)
                    {
                        // If it's my turn, use the ucb value that maximize my winrate
                        score = scores[n->id].get_ucb(scores[root.id].played);
                    }
                    else
                    {
                        // If it's my opponent's turn, use the ucb value that minimize my winrate
                        score = scores[n->id].get_inverse_ucb(scores[root.id].played);
                    }

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

            // 4) Backpropagation
            // Backpropagate the outcome to parent node until we reach the root node
            while (node != NULL)
            {
                // Update node's values
                scores[node->id].increase(gain, played);

                // Go to parent
                node = node->parent;
            }
        }

// Reduce the scores
#pragma omp critical
        {
            global_scores[root.id].increase(scores[root.id].score, scores[root.id].played);
            for (int i = 0; i < root.possible_moves->size(); i++)
            {
                global_scores[root.children[i]->id].increase(scores[root.children[i]->id].score, scores[root.children[i]->id].played);
            }
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl
                  << "Played: " << global_scores[global_root.id].played << std::endl
                  << "Current win rate: " << (global_scores[global_root.id].get_winrate() * 100) << "%" << std::endl;
    }

    // Now we can select the move with the highest winrate

    Node *selected_node;
    float highscore = -1;

    // Select the node children of root with the best winrate
    for (int i = 0; i < global_root.possible_moves->size(); i++)
    {
        Node *n = global_root.children[i];
        float ucb = global_scores[n->id].get_ucb(global_scores[global_root.id].played);
        float winrate = global_scores[n->id].get_winrate();
        if (winrate > highscore)
        {
            selected_node = n;
            highscore = winrate;
        }

        if (verbose)
            std::cout << n->move->to_string() << ": " << (winrate * 100) << "% played: " << global_scores[n->id].played << " ucb: " << ucb << std::endl;
    }

    Engine::IMove *selected_move = selected_node->move->get_copy();

    return selected_move;
}
