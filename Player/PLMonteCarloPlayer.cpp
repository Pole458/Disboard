#include "PLMonteCarloPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <string>
#include <chrono>


PLMonteCarloPlayer::PLMonteCarloPlayer(int rollouts, int rollout_size, bool verbose)
{
    this->rollouts = rollouts;
    this->verbose = verbose;
    this->rollout_size = rollout_size;
}

Engine::IMove *PLMonteCarloPlayer::choose_move(Engine::IBoard *board)
{
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int my_turn = board->turn % 2;

    int max_depth_reached = 0;

    // Creates root node for the game-tree starting from the current board configuration.
    Node root = Node(board->get_copy());

    // Clear cached values
    nodes.clear();
    scores.clear();
    gains.clear();
    
    Node *node;

    omp_lock_t debug;
    omp_init_lock(&debug);

    bool should_stop = false;
   
    #pragma omp parallel
    {
        while(!should_stop)
        {
            #pragma omp master
            {
                node = &root;

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

                // omp_set_lock(&debug);
                // std::cout << omp_get_thread_num() << " selected node " << node->id << std::endl;
                // omp_unset_lock(&debug);

                if (depth > max_depth_reached)
                    max_depth_reached = depth;
            
                float gain = 0;

                if(node->is_leaf())
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

                    // Set this node to be updated in backprop
                    gains[node->id] += gain;
                    playeds[node->id] += 1;
                }
                else
                {
                    // Prepare expansion
                    node->children = new Node*[node->possible_moves->size()];
                    node->expanded = true;
                }
            }

            #pragma omp barrier

            // Parallel section

            if(!node->is_leaf())
            {
                // 2) Expansion & Rollout
                // If this node it's not a terminal node, expand it and simulate a random game for each child from theirs board configurations
                #pragma omp for
                for(int i = 0; i < node->possible_moves->size(); i++)
                {

                    // omp_set_lock(&debug);
                    // std::cout << omp_get_thread_num() << " rollout child " << i << std::endl;
                    // omp_unset_lock(&debug);
        
                    // Create one children node
                    Node *child = new Node(board->get_copy(), node->possible_moves->move_at(i), node);
                    node->children[i] = child;

                    // Rollout
                    Engine::IBoard *test_board = node->board->get_copy();
                    Engine::play(test_board, &player, &player);
                
                    float gain = 0;

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

                    #pragma omp critical
                    {

                        nodes[child->id].emplace(child);

                        // Set this node to be updated in backprop
                        gains[child->id] += gain;
                        playeds[child->id] += 1;

                        // std::cout << omp_get_thread_num() << " child " << i << " " << child->id << ": " << scores[child->id].score << " / " << scores[child->id].played << std::endl;
                    }
                }
            }

            #pragma omp barrier

            #pragma omp master
            {
                // std::cout << omp_get_thread_num() << " backprop " << global_gain << " " << global_played << std::endl;

                // 4) Backpropagation
                // Backpropagate the outcome to parent node until we reach the root node
                backprop();

                // Check loop condition
                should_stop = scores[root.id].played >= rollouts;

                std::cout << omp_get_thread_num() << " rollouts: " << scores[root.id].played << std::endl;
            }

            #pragma omp barrier
        }
    }
 
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (verbose)
    {
        std::cout << "Max Depth: " << max_depth_reached << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl
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

void PLMonteCarloPlayer::backprop()
{
  
    // 4) Backpropagation
    // Backpropagate the outcome to parent node until we reach the root node
    while (!gains.empty())
    {

        Engine::board_id id = gains.begin()->first;
        float gain = gains.begin()->second;
        int played = playeds.begin()->second;

        // std::cout << "backprop " << id << ": " << gain << " / " << played << std::endl;

        // Increase score for all nodes with this id
        scores[id].increase(gain, played);

        // Get all nodes with this id
        std::unordered_set<Node*> node_set = nodes[id];

        for (auto it = node_set.begin(); it != node_set.end(); ++it)
        {
            Node* node = *it;
        
            // Add parent id to the ones to increase
            if(node->parent != NULL)
            {
                if(node->parent->id == node->id)
                {
                    // std::cout << " same id as parent\n" << node->board->to_string() << std::endl << node->parent->board->to_string() << std::endl << node->parent->move->to_string() << std::endl;

                    // if(node->parent == node)
                    // {
                    //     std::cout << " parent is me" << std::endl;
                    // }
                }
                else
                {
                    gains[node->parent->id] += gain;
                    playeds[node->parent->id] += played;
                    std::cout << " adding " << node->parent->id << std::endl;
                }
            }
        }

        // Remove id from the ones to check
        gains.erase(id);
        playeds.erase(id);

        // std::cin.get();
    }
}