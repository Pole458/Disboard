#include "MonteCarloPlayer.h"

MonteCarloPlayer::MonteCarloPlayer(float thinking_time, bool verbose)
{
    this->thinking_time = thinking_time;
    this->verbose = verbose;   
}

Engine::IMove *MonteCarloPlayer::choose_move(Engine::IBoard *board)
{

    int my_turn = board->turn % 2;

    // Reset analyitics
    max_depth_reached = 0;
    iterations = 0;

    // Internal random player used to for random rollout
    RandomPlayer random_player;

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, Score> scores;

    // Hash map used to group nodes with the same id
    std::unordered_map<Engine::board_id, std::unordered_set<Node*>> nodes;

    // Hash map of scores used for backpropagation
    std::unordered_map<Engine::board_id, Score> back_prop;

    // Creates root node for the game-tree starting from the current board configuration.
    Node root = Node(board->get_copy());

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() < thinking_time)
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

        if (!node->is_leaf())
        {
            node->expand();

            for(int i = 0; i < node->possible_moves->size(); i++)
            {
                nodes[node->children[i]->id].insert(node->children[i]);
            }

            // Pick one children node
            node = node->children[random_player.rng(node->possible_moves->size())];

            // 3) Rollout
            // Simulate a random game from the node's board configuration

            Engine::IBoard *test_board = node->board->get_copy();
            Engine::play(test_board, &random_player, &random_player);

            // Node the board is in a terminal state, get the outcome
            if (test_board->status == Engine::IBoard::Draw)
            {
                back_prop[node->id].increase(0.5f, 1);
            }
            else if ((my_turn == 1 && test_board->status == Engine::IBoard::First) || (my_turn == 0 && test_board->status == Engine::IBoard::Second))
            {
                back_prop[node->id].increase(1, 1);
            }
            else
            {
                back_prop[node->id].increase(0, 1);
            }

            delete test_board;
        }
        else
        {
            // Node it's a terminal node, get the outcome
            if (node->board->status == Engine::IBoard::Draw)
            {
                back_prop[node->id].increase(0.5f, 1);
            }
            else if ((my_turn == 1 && node->board->status == Engine::IBoard::First) || (my_turn == 0 && node->board->status == Engine::IBoard::Second))
            {
                back_prop[node->id].increase(1, 1);
            }
            else
            {
                back_prop[node->id].increase(0, 1);
            }
        }

        // 4) Backpropagation
        // Backpropagate the outcome to parent node until we reach the root node

        while (!back_prop.empty())
        {
            std::unordered_map<Engine::board_id, Score> to_do;

            // Prepare to-do maps
            for(auto it = back_prop.begin(); it != back_prop.end(); it++)
            {
                Engine::board_id id = it->first;
                to_do.emplace(id, it->second);
            }

            back_prop.clear();

            for(auto it = to_do.begin(); it != to_do.end(); it++)
            {
                Engine::board_id id = it->first;
                Score score = it->second;

                // Increase score for all nodes with this id
                scores[id].increase(score);

                // Get all nodes with this id
                std::unordered_set<Node*> *node_set = &nodes[id];

                for (auto it = node_set->begin(); it != node_set->end(); ++it)
                {
                    Node* node = *it;
                
                    // Add parent id to the ones to increase
                    if(node->parent != NULL)
                    {
                        back_prop[node->parent->id].increase(score);
                    }
                }
            }
        }

        iterations++;
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl
                  << "Iterations: " << iterations << std::endl
                  << "Played: " << scores[root.id].played << std::endl
                  << "Current win rate: " << (scores[root.id].get_winrate() * 100) << "%" << std::endl;
    }

    // Now we can select the move with the highest winrate

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
