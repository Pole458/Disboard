#include "PYBWMiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <iostream>
#include <chrono>

PYBWMiniMaxPlayer::PYBWMiniMaxPlayer(int depth, bool verbose, int cache_size) : MiniMaxPlayer(depth, verbose, cache_size)
{
    for(int i = 0; i < omp_get_max_threads(); i++)
    {
        random_players.emplace_back();
    }
}

Engine::IMove *PYBWMiniMaxPlayer::choose_move(Engine::IBoard *board)
{

    my_turn = board->turn % 2;

    // Reset analytcs values
    nodes_evaluated = 0;
    leafs_reached = 0;
    nodes_pruned = 0;
    cache_hits = 0;

    float highscore = std::numeric_limits<float>::lowest();
    float alpha = std::numeric_limits<float>::lowest();
    float beta = std::numeric_limits<float>::max();

    Node root(board->get_copy());
    root.expand();
    Engine::IMove *selected_move = NULL;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Move ordering
    int indexes[root.possible_moves->size()] = {};
    float heuristic_scores[root.possible_moves->size()] = {};
    for(int i = 0; i < root.possible_moves->size(); i++)
    {
        int pos = i;
        float fast_score = get_heuristic_score(root.children[i], 10);
        for(; pos > 0 && heuristic_scores[pos - 1] < fast_score; pos--)
        {   
            indexes[pos] = indexes[pos - 1];
            heuristic_scores[pos] = heuristic_scores[pos - 1];
        }
        indexes[pos] = i;
        heuristic_scores[pos] = fast_score;
    }

    // Test best node to get alpha and beta values
    Node* child = root.children[indexes[0]];
    float score = pminimize(child, max_depth - 1, alpha, beta);

    selected_move = child->move;
    highscore = std::max(score, highscore);
    alpha = std::max(alpha, highscore);

    if (verbose)
    {
        if(score > 1)
        {
            std::cout << child->move->to_string() << " win in " << (std::round(1 / (score - 1)) + 1 - board->turn) << " turns" << std::endl;
        }
        else if(score < 0)
        {
            std::cout << child->move->to_string() << " loss in " << (std::round(-1 / score) + 1 - board->turn) << " turns" << std::endl;
        }
        else
        {
            std::cout << child->move->to_string() << " winrate: " << score * 100 << "%" << std::endl;
        }
    }

    omp_lock_t score_lock;
    omp_init_lock(&score_lock);

    // Search for the children node with the maximum score
    #pragma omp parallel for private(child)
    for (int i = 1; i < root.possible_moves->size(); i++)
    {
        child = root.children[indexes[i]];
        float score = minimize(child, max_depth - 1, alpha, beta);

        #pragma omp critical(cout)
        if (verbose)
        {
            std::cout << child->move->to_string();

            if(score > 1)
            {
                std::cout << " win in " << (std::round(1 / (score - 1)) + 1 - board->turn) << " turns";
            }
            else if(score < 0)
            {
                std::cout << " loss in " << (std::round(-1 / score) + 1 - board->turn) << " turns";
            }
            else
            {
                std::cout << " winrate: " << score * 100 << "%";
            }

            std::cout << std::endl;
        }

        omp_set_lock(&score_lock);
        if (score > highscore)
        {
            highscore = score;
            selected_move = child->move;
            alpha = std::max(alpha, highscore);
        }
        omp_unset_lock(&score_lock);
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


float PYBWMiniMaxPlayer::pmaximize(Node *node, int depth, float alpha, float beta)
{

    // If this board position has already been evaluated, return the cached value
    if (is_id_cached(node->id))
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
        float score = get_heuristic_score(node, 100); 
        set_cached_score(node->id, score);
        return score;
    }

    nodes_evaluated++;

    float highscore = std::numeric_limits<float>::lowest();

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

    // Move ordering and cache check for pruning
    int indexes[node->possible_moves->size()] = {};
    float heuristic_scores[node->possible_moves->size()] = {};
    int nodes_to_check = 0;
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_cached(node->children[i]->id))
        {
            cache_hits++;
            highscore = std::max(highscore, get_cached_score(node->children[i]->id));
            alpha = std::max(alpha, highscore);
            if (beta <= alpha)
            {
                break;
            }
        }
        else
        {
            int pos = nodes_to_check;
            float fast_score = get_heuristic_score(node->children[i], 10);
            for(; pos > 0 && heuristic_scores[pos - 1] < fast_score; pos--)
            {   
                indexes[pos] = indexes[pos - 1];
                heuristic_scores[pos] = heuristic_scores[pos - 1];
            }
            indexes[pos] = i;
            heuristic_scores[pos] = fast_score;
            nodes_to_check++;
        }
    }

    // Prune search
    alpha = std::max(alpha, highscore);
    if (beta <= alpha)
    {
        nodes_pruned++;
    }
    else
    {
        // Search best node first to get alpha beta values
        highscore = std::max(highscore, pminimize(node->children[indexes[0]], depth - 1, alpha, beta));
        
        omp_lock_t score_lock;
        omp_init_lock(&score_lock);

        bool pruned = false;
        // Search in other nodes not yet explored
        #pragma omp parallel for
        for (int i = 1; i < nodes_to_check; i++)
        {
            if(pruned) continue;
            
            highscore = std::max(highscore, minimize(node->children[indexes[i]], depth - 1, alpha, beta));

            // Prune search
            
            omp_set_lock(&score_lock);
            alpha = std::max(alpha, highscore);
            if (beta <= alpha)
            {
                nodes_pruned++;
                pruned = true;
            }
            omp_unset_lock(&score_lock);
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

float PYBWMiniMaxPlayer::pminimize(Node *node, int depth, float alpha, float beta)
{

    // If this board position has already been evaluated, return the cached value
    if (is_id_cached(node->id))
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
        float score = get_heuristic_score(node, 100); 
        set_cached_score(node->id, score);
        return score;
    }

    nodes_evaluated++;

    float lowscore = std::numeric_limits<float>::max();

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

    // Move ordering and cache check for pruning
    int indexes[node->possible_moves->size()] = {};
    float heuristic_scores[node->possible_moves->size()] = {};
    int nodes_to_check = 0;
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_cached(node->children[i]->id))
        {
            cache_hits++;
            lowscore = std::min(lowscore, get_cached_score(node->children[i]->id));
            beta = std::min(beta, lowscore);
            if (beta <= alpha)
            {
                break;
            }
        }
        else
        {
            int pos = nodes_to_check;
            float fast_score = get_heuristic_score(node->children[i], 10);
            for(; pos > 0 && heuristic_scores[pos - 1] > fast_score; pos--)
            {   
                indexes[pos] = indexes[pos - 1];
                heuristic_scores[pos] = heuristic_scores[pos - 1];
            }
            indexes[pos] = i;
            heuristic_scores[pos] = fast_score;
            nodes_to_check++;
        }
    }

    // Prune search
    beta = std::min(beta, lowscore);
    if (beta <= alpha)
    {
        nodes_pruned++;
    }
    else
    {

        // Search best node first to get alpha beta values
        lowscore = std::min(lowscore, pmaximize(node->children[indexes[0]], depth - 1, alpha, beta));

        omp_lock_t score_lock;
        omp_init_lock(&score_lock);

        bool pruned = false;
        // Search for the children node with the minimum score
        #pragma omp parallel for
        for (int i = 1; i < nodes_to_check; i++)
        {
            if(pruned) continue;

            lowscore = std::min(lowscore, maximize(node->children[indexes[i]], depth - 1, alpha, beta));

            // Prune search
            omp_set_lock(&score_lock);
            {
                beta = std::min(beta, lowscore);
                if (beta <= alpha)
                {
                    nodes_pruned++;
                    pruned = true;
                }
            }
            omp_unset_lock(&score_lock);
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

float PYBWMiniMaxPlayer::maximize(Node *node, int depth, float alpha, float beta)
{

    // If this board position has already been evaluated, return the cached value
    if (is_id_cached(node->id))
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
        float score = get_heuristic_score(node, 100); 
        set_cached_score(node->id, score);
        return score;
    }

    nodes_evaluated++;

    float highscore = std::numeric_limits<float>::lowest();

    node->expand();

    // Move ordering and cache check for pruning
    int indexes[node->possible_moves->size()] = {};
    float heuristic_scores[node->possible_moves->size()] = {};
    int nodes_to_check = 0;
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_cached(node->children[i]->id))
        {
            cache_hits++;
            highscore = std::max(highscore, get_cached_score(node->children[i]->id));
            // Prune search
            alpha = std::max(alpha, highscore);
            if (beta <= alpha)
            {
                nodes_pruned++;
                node->reduce();
                set_cached_score(node->id, highscore);
                return highscore;
            }
        }
        else
        {
            int pos = nodes_to_check;
            float fast_score = get_heuristic_score(node->children[i], 10);
            for(; pos > 0 && heuristic_scores[pos - 1] < fast_score; pos--)
            {   
                indexes[pos] = indexes[pos - 1];
                heuristic_scores[pos] = heuristic_scores[pos - 1];
            }
            indexes[pos] = i;
            heuristic_scores[pos] = fast_score;
            nodes_to_check++;
        }
    }
    
    // Search in other nodes not yet explored
    for (int i = 0; i < nodes_to_check; i++)
    {
        highscore = std::max(highscore, minimize(node->children[indexes[i]], depth - 1, alpha, beta));

        // Prune search
        alpha = std::max(alpha, highscore);
        if (beta <= alpha)
        {
            nodes_pruned++;
            break;
        }
    }

    node->reduce();

    set_cached_score(node->id, highscore);

    return highscore;
}

float PYBWMiniMaxPlayer::minimize(Node *node, int depth, float alpha, float beta)
{

    // If this board position has already been evaluated, return the cached value
    if (is_id_cached(node->id))
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
        float score = get_heuristic_score(node, 100); 
        set_cached_score(node->id, score);
        return score;
    }

    nodes_evaluated++;

    float lowscore = std::numeric_limits<float>::max();

    node->expand();

    // Move ordering and cache check for pruning
    int indexes[node->possible_moves->size()] = {};
    float heuristic_scores[node->possible_moves->size()] = {};
    int nodes_to_check = 0;
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_cached(node->children[i]->id))
        {
            cache_hits++;
            lowscore = std::min(lowscore, get_cached_score(node->children[i]->id));
            // Prune search
            beta = std::min(beta, lowscore);
            if (beta <= alpha)
            {
                nodes_pruned++;
                node->reduce();
                set_cached_score(node->id, lowscore);
                return lowscore;
            }
        }
        else
        {
            int pos = nodes_to_check;
            float fast_score = get_heuristic_score(node->children[i], 10);
            for(; pos > 0 && heuristic_scores[pos - 1] > fast_score; pos--)
            {   
                indexes[pos] = indexes[pos - 1];
                heuristic_scores[pos] = heuristic_scores[pos - 1];
            }
            indexes[pos] = i;
            heuristic_scores[pos] = fast_score;
            nodes_to_check++;
        }
    }
   
    // Search for the children node with the minimum score
    for (int i = 0; i < nodes_to_check; i++)
    {
        lowscore = std::min(lowscore, maximize(node->children[indexes[i]], depth - 1, alpha, beta));

        // Prune search
        beta = std::min(beta, lowscore);
        if (beta <= alpha)
        {
            nodes_pruned++;
            break;
        }
    }
    node->reduce();

    set_cached_score(node->id, lowscore);

    return lowscore;
}

bool PYBWMiniMaxPlayer::is_id_cached(Engine::board_id id)
{
    bool b;
    #pragma omp critical(cache)
    {
        auto it = cached_id.find(id % cache_size);
        b = (it != cached_id.end() && it->second == id);
    }
    return b;
}

float PYBWMiniMaxPlayer::get_cached_score(Engine::board_id id)
{
    float score;
    
    #pragma omp critical(cache)
    score = cached_scores.at(id % cache_size);

    return score;
}

void PYBWMiniMaxPlayer::set_cached_score(Engine::board_id id, float score)
{
    #pragma omp critical(cache)
    {
        cached_id[id % cache_size] = id;
        cached_scores[id % cache_size] = score;
    }
}

float PYBWMiniMaxPlayer::get_heuristic_score(Node *node, int rollouts)
{
    float score = 0;;

    RandomPlayer* player = &random_players.at(omp_get_thread_num());

    #pragma omp parallel for reduction(+:score)
    for (int i = 0; i < rollouts; i++)
    {
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

    return score / rollouts;
}
