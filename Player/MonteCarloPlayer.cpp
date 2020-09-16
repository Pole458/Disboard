#include "MonteCarloPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <chrono>

MonteCarloPlayer::MonteCarloPlayer(int rollouts, bool verbose)
{
    this->rollouts = rollouts;
    this->verbose = verbose;

    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    // Make a random number engine
    rng = pcg32(seed_source);
}

Engine::IMove *MonteCarloPlayer::choose_move(Engine::IBoard *board)
{
    
    int my_turn = board->turn % 2;

    int max_depth_reached = 0;

    int iterations = 0;

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, Score> scores;

    // Creates root node for the game-tree starting from the current board configuration.
    Node root = Node(board->get_copy());

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    while (iterations < rollouts)
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
            float highscore = -1;
            float score;

            if (node->board->turn % 2 == my_turn)
            {
                // If it's my turn, use the ucb value that maximize my winrate
                for (int i = 0; i < node->possible_moves->size(); i++)
                {
                    Node *n = node->children[i];
                    score = scores[n->id].get_ucb(scores[root.id].played);
                    if (score > highscore)
                    {
                        selected_node = n;
                        highscore = score;
                    }
                }
            }
            else
            {
                // If it's my opponent's turn, use the ucb value that minimize my winrate
                for (int i = 0; i < node->possible_moves->size(); i++)
                {
                    Node *n = node->children[i];
                    score = scores[n->id].get_inverse_ucb(scores[root.id].played);
                    if (score > highscore)
                    {
                        selected_node = n;
                        highscore = score;
                    }
                }
            }

            depth++;
            node = selected_node;
        }

        if (max_depth_reached < depth)
            max_depth_reached = depth;

        // 2) Expansion
        // If this node it's not a terminal node, expand it

        float gain = 0;
        int played = 1;

        if (!node->is_leaf())
        {
            node->expand();

            // Pick one children node
            node = node->children[rng(node->possible_moves->size())];

            // 3) Rollout
            // Simulate a random game from the node's board configuration

            Engine::IBoard *test_board = node->board->get_copy();
            Engine::play(test_board, &player, &player);

            // Node the board is in a terminal state, get the outcome
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
            scores[node->id].score += gain;
            scores[node->id].played += played;

            // Go to parent
            node = node->parent;
        }

        iterations++;
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // Now we can select the move with the highest winrate

    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Current win rate: " << (scores[root.id].get_winrate() * 100) << "%" << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl;
    }

    Node *selected_node;
    float highscore = -1;

    // Select the node children of root with the best winrate
    for (int i = 0; i < root.possible_moves->size(); i++)
    {
        Node *n = root.children[i];
        float ucb = scores[n->id].get_ucb(scores[root.id].played);
        float winrate = scores[n->id].get_winrate();
        if (winrate > highscore)
        {
            selected_node = n;
            highscore = winrate;
        }

        if (verbose)
            std::cout << n->move->to_string() << ": " << (winrate * 100) << "% played: " << scores[n->id].played << " ucb: " << ucb << std::endl;
    }

    Engine::IMove *selected_move = selected_node->move->get_copy();

    return selected_move;
}
