#include "PMiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <iostream>
#include <chrono>

PMiniMaxPlayer::PMiniMaxPlayer(int depth, bool verbose, int cache_size)
{
    this->max_depth = depth;
    this->verbose = verbose;
    this->cache_size = cache_size;

    for(int i = 0; i < omp_get_max_threads(); i++)
    {
        random_players.emplace_back();
    }
}

Engine::IMove *PMiniMaxPlayer::choose_move(Engine::IBoard *board)
{

    my_turn = board->turn % 2;

    // Reset analytcs values
    nodes_evaluated = 0;
    leafs_reached = 0;
    nodes_pruned = 0;
    cache_hits = 0;

    int alpha = std::numeric_limits<int>::lowest();
    int beta = std::numeric_limits<int>::max();
    int highscore = std::numeric_limits<int>::lowest();

    Node root(board->get_copy());
    root.expand();
    Engine::IMove *selected_move = NULL;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Search for the children node with the maximum score
    for (int i = 0; i < root.possible_moves->size(); i++)
    {
        Node* child = root.children[i];
        int score = minimize(child, max_depth - 1, alpha, beta);

        if (score > highscore)
        {
            highscore = score;
            selected_move = child->move;
            alpha = std::max(alpha, highscore);
        }

        if (verbose)
        {
            std::cout << child->move->to_string() << " score: " << score << std::endl;
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (verbose)
    {
        std::cout
            << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl
            << "Nodes evaluated " << nodes_evaluated << std::endl
            << "Leafs reached " << leafs_reached << std::endl   
            << "Nodes pruned: " << nodes_pruned << std::endl
            << "Cache hits " << cache_hits << std::endl;
    }

    // Clear cache
    cached_id.clear();
    cached_scores.clear();

    return selected_move->get_copy();
}


float PMiniMaxPlayer::maximize(Node *node, int depth, float alpha, float beta)
{

    // If this board position has already been evaluated, return the cached value
    if (is_id_scored(node->id))
    {
        cache_hits++;
        return get_cached_score(node->id);
    }

    // If node is terminal, return score
    if(node->is_leaf())
    {
        leafs_reached++;
        float score = get_score(node);
        set_cached_score(node->id, score);
        return score;
    }

    // If max depth was reached, get heuristic score
    if (depth == 0)
    {
        leafs_reached++;
        float score = get_heuristic_score(node); 
        set_cached_score(node->id, score);
        return score;
    }

    nodes_evaluated++;

    // Parallel expansion
    if(!node->expanded)
    {
        node->children = new Node*[node->possible_moves->size()];

        #pragma omp parallel for
        for (int i = 0; i < node->possible_moves->size(); i++)
        {
            node->children[i] = new Node(node->board->get_copy(), node->possible_moves->move_at(i), node);;
        }

        node->expanded = true;
    }

    float highscore = std::numeric_limits<float>::lowest();

    // Search first for cached results in order to optimize pruning
    #pragma omp parallel for reduction(max:highscore)
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_scored(node->children[i]->id))
        {
            highscore = std::max(highscore, get_cached_score(node->children[i]->id));
        }
    }
    
    // Can we prune?
    alpha = std::max(alpha, highscore);
    if (beta <= alpha)
    {
        nodes_pruned++;      
    }
    else
    {
        // Search in other nodes not yet explored
        for (int i = 0; i < node->possible_moves->size(); i++)
        {
            highscore = std::max(highscore, minimize(node->children[i], depth - 1, alpha, beta));

            // Prune search
            alpha = std::max(alpha, highscore);
            if (beta <= alpha)
            {
                nodes_pruned++;
                break;
            }
        }
    }
    
    // Parallel reduction
    if(!node->expanded)
    {
        #pragma omp parallel for
        for(int i = 0; i < node->possible_moves->size(); i++)
        {
            delete node->children[i];
        }

        delete[] node->children;

        node->expanded = false;
    }

    set_cached_score(node->id, highscore);

    return highscore;
}

float PMiniMaxPlayer::minimize(Node *node, int depth, float alpha, float beta)
{

    // If this board position has already been evaluated, return the cached value
    if (is_id_scored(node->id))
    {
        cache_hits++;
        return get_cached_score(node->id);
    }

    // If node is terminal, return score
    if(node->is_leaf())
    {
        leafs_reached++;
        float score = get_score(node);
        set_cached_score(node->id, score);
        return score;
    }

    // If max depth was reached, get heuristic score
    if (depth == 0)
    {
        leafs_reached++;
        float score = get_heuristic_score(node); 
        set_cached_score(node->id, score);
        return score;
    }

    nodes_evaluated++;

    // Parallel expansion
    if(!node->expanded)
    {
        node->children = new Node*[node->possible_moves->size()];

        #pragma omp parallel for
        for (int i = 0; i < node->possible_moves->size(); i++)
        {
            node->children[i] = new Node(node->board->get_copy(), node->possible_moves->move_at(i), node);;
        }

        node->expanded = true;
    }

    float lowscore = std::numeric_limits<float>::max();

    // Search first for cached results in order to optimize pruning
    #pragma omp parallel for reduction(min:lowscore)
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_scored(node->children[i]->id))
        {
            lowscore = std::max(lowscore, get_cached_score(node->children[i]->id));
        }
    }
   
    // Can we prune?
    beta = std::min(beta, lowscore);
    if (beta <= alpha)
    {
        nodes_pruned++;       
    }
    else
    {
        // Search for the children node with the minimum score
        for (int i = 0; i < node->possible_moves->size(); i++)
        {
            lowscore = std::min(lowscore, maximize(node->children[i], depth - 1, alpha, beta));

            // Prune search
            beta = std::min(beta, lowscore);
            if (beta <= alpha)
            {
                nodes_pruned++;
                break;
            }
        }
    }

    // Parallel reduction
    if(!node->expanded)
    {
        #pragma omp parallel for
        for(int i = 0; i < node->possible_moves->size(); i++)
        {
            delete node->children[i];
        }

        delete[] node->children;

        node->expanded = false;
    }

    set_cached_score(node->id, lowscore);

    return lowscore;
}

bool PMiniMaxPlayer::is_id_scored(Engine::board_id id)
{
    auto it = cached_id.find(id % cache_size);
    return it != cached_id.end() && it->second == id;
}

float PMiniMaxPlayer::get_cached_score(Engine::board_id id)
{
    return cached_scores.at(id % cache_size);
}

void PMiniMaxPlayer::set_cached_score(Engine::board_id id, float score)
{
    cached_id[id % cache_size] = id;
    cached_scores[id % cache_size] = score;
}

float PMiniMaxPlayer::get_score(Node* node)
{
    if (node->board->status == Engine::IBoard::Draw)
    {
        // Draw
        return 50;
    }
    else if ((my_turn == 1 && node->board->status == Engine::IBoard::First) || (my_turn == 0 && node->board->status == Engine::IBoard::Second))
    {
        // Win
        return 100 + 1.0f / node->board->turn;
    }

    // Loss
    return - 1.0f / node->board->turn;
}

float PMiniMaxPlayer::get_heuristic_score(Node *node)
{
    float score = 0;

    #pragma omp parallel for reduction(+:score)
    for (int i = 0; i < 100; i++)
    {
        RandomPlayer* player = &random_players.at(omp_get_thread_num());

        Engine::IBoard *test_board = node->board->get_copy();
        Engine::play(test_board, player, player);

        if (test_board->status == Engine::IBoard::Draw)
        {
            score += 0.5f;
        }
        else if ((my_turn == 1 && test_board->status == Engine::IBoard::First) || (my_turn == 0 && test_board->status == Engine::IBoard::Second))
        {
            score++;
        }

        delete test_board;
    }

    return score;
}
