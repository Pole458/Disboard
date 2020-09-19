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
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int my_turn = board->turn % 2;

    int max_depth_reached = 0;

    omp_lock_t debug;
    omp_init_lock(&debug);

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, PScore> scores;

    // Hash map used to group nodes with the same id
    std::unordered_map<Engine::board_id, std::unordered_set<PNode*>> nodes;

    // Hash map of scores used for backpropagation
    std::unordered_map<Engine::board_id, PScore> back_prop;    

    // Creates root node for the game-tree starting from the current board configuration.
    PNode root = PNode(board->get_copy());

    #pragma omp parallel for reduction(max:max_depth_reached)
    for (int iterations = 0; iterations < rollouts; iterations++)
    {
        PNode *node = &root;
        PNode *explored_node;
        
        // 1) Tree traversing
        // Search for the most interesting node to be tested.
        // Starting from the root node, select the children node with the best ucb value
        // Repeat until we found a node not yet expanded

        int depth = 0;

        #pragma omp critical
        {
            // Keep searching for a node not yet expanded
            while (node->is_expanded())
            {

                PNode *selected_node;
                float highscore = std::numeric_limits<int>::min();
                float score;

                for (int i = 0; i < node->possible_moves->size(); i++)
                {
                    PNode *n = node->children[i];

                    if(n->is_being_explored())
                    {
                        score = -1;
                    }
                    else
                    {
                        #pragma omp critical(scores)
                        {
                            if (node->board->turn % 2 == my_turn)
                            {
                                // If it's my turn, use the ucb value that maximize my winrate
                                score = scores[n->id].get_ucb(scores.at(root.id).get_played());
                            }
                            else
                            {
                                // If it's my opponent's turn, use the ucb value that minimize my winrate
                                score = scores[n->id].get_inverse_ucb(scores.at(root.id).get_played());
                            }
                        }                        
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

            explored_node = node;
            explored_node->set_explored(true);
        }

        if (depth > max_depth_reached)
            max_depth_reached = depth;

        // 2) Expansion
        // If this node it's not a terminal node, expand it

        if (!node->is_leaf())
        {
            // Expand node
            node->expand();

            #pragma omp critical(nodes)
            {
                for(int i = 0; i < node->possible_moves->size(); i++)
                {
                    nodes[node->children[i]->id].insert(node->children[i]);
                }
            }
         
            // Pick one children node
            node = node->children[0];

            // 3) Rollout
            // Simulate a random game from the node's board configuration

            Engine::IBoard *test_board = node->board->get_copy();
            Engine::play(test_board, &player, &player);

            #pragma omp critical(back_prob)
            {
                // The board is now in a terminal state, get the outcome
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
            }
           
            delete test_board;
        }
        else
        {
            #pragma omp critical(back_prob)
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
        }

        // 4) Backpropagation
        // Backpropagate the outcome to parent node until we reach the root node

        while (!back_prop.empty())
        {
            std::unordered_map<Engine::board_id, PScore> to_do;

            // Prepare to-do maps
            for(auto it = back_prop.begin(); it != back_prop.end(); it++)
            {
                Engine::board_id id = it->first;
                to_do.emplace(id, it->second);
            }

            #pragma omp critical(back_prop)
            {
                back_prop.clear();
            }
            
            for(auto it = to_do.begin(); it != to_do.end(); it++)
            {
                Engine::board_id id = it->first;
                PScore score = it->second;

                // Increase score for all nodes with this id
                #pragma omp critical(scores)
                {
                    scores[id].increase(score);
                }

                // Get all nodes with this id
                std::unordered_set<PNode*> *node_set = &nodes[id];

                for (auto it = node_set->begin(); it != node_set->end(); ++it)
                {
                    PNode* node = *it;
                
                    // Add parent id to the ones to increase
                    if(node->parent != NULL)
                    {
                        #pragma omp critical(back_prop)
                        {
                            back_prop[node->parent->id].increase(score);
                        }
                    }
                }
            }
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
