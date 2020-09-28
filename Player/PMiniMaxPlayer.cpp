#include "PMiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"

PMiniMaxPlayer::PMiniMaxPlayer(int depth, bool verbose, int cache_size) : MiniMaxPlayer(depth, verbose, cache_size)
{
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

    float alpha = std::numeric_limits<float>::lowest();
    float beta = std::numeric_limits<float>::max();
    float highscore = std::numeric_limits<float>::lowest();

    Node root(board->get_copy());
    root.expand();
    Engine::IMove *selected_move = NULL;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Move ordering
    int indexes[root.possible_moves->size()] = {};
    float heuristic_scores[root.possible_moves->size()] = {};
    for(int i = 0, size = 0; size < root.possible_moves->size(); i++)
    {
        if(!is_id_cached(root.children[i]->id))
        {
            int pos = size;
            float fast_score = get_heuristic_score(root.children[i], 10);
            for(; pos > 0 && heuristic_scores[pos - 1] < fast_score; pos--)
            {   
                indexes[pos] = indexes[pos - 1];
                heuristic_scores[pos] = heuristic_scores[pos - 1];
            }
            indexes[pos] = i;
            heuristic_scores[pos] = fast_score;
            size++;
        }
    }

    // Search for the children node with the maximum score
    for (int i = 0; i < root.possible_moves->size(); i++)
    {
        Node* child = root.children[indexes[i]];
        float score = minimize(child, max_depth - 1, alpha, beta);

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

        if (score > highscore)
        {
            highscore = score;
            selected_move = child->move;
            alpha = std::max(alpha, highscore);
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

    int nodes_to_explore = 0;
    int ch = 0;

    // Search first for cached results in order to optimize pruning
    #pragma omp parallel for reduction(max:highscore) reduction(+:nodes_to_explore) reduction(+:ch)
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_cached(node->children[i]->id))
        {
            highscore = std::max(highscore, get_cached_score(node->children[i]->id));
            ch++;
        }
        else
        {
            nodes_to_explore++;
        }
    }
    
    cache_hits += ch;

    // Can we prune?
    alpha = std::max(alpha, highscore);
    if (beta <= alpha)
    {
        nodes_pruned++;
    }
    else
    {
        // Move ordering and cache check for pruning
        int indexes[nodes_to_explore] = {};
        float heuristic_scores[nodes_to_explore] = {};
        for(int i = 0, size = 0; size < nodes_to_explore; i++)
        {
            if(!is_id_cached(node->children[i]->id))
            {
                int pos = size;
                float fast_score = get_heuristic_score(node->children[i], 10);
                for(; pos > 0 && heuristic_scores[pos - 1] < fast_score; pos--)
                {   
                    indexes[pos] = indexes[pos - 1];
                    heuristic_scores[pos] = heuristic_scores[pos - 1];
                }
                indexes[pos] = i;
                heuristic_scores[pos] = fast_score;
                size++;
            }
        }

        // Search in other nodes not yet explored
        for (int i = 0; i < nodes_to_explore; i++)
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

    int nodes_to_explore = 0;
    int ch = 0;

    // Search first for cached results in order to optimize pruning
    #pragma omp parallel for reduction(min:lowscore) reduction(+:nodes_to_explore) reduction(+:ch)
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        if(is_id_cached(node->children[i]->id))
        {
            lowscore = std::min(lowscore, get_cached_score(node->children[i]->id));
            ch++;
        }
        else
        {
            nodes_to_explore++;
        }
    }
   
    cache_hits += ch;

    // Can we prune?
    beta = std::min(beta, lowscore);
    if (beta <= alpha)
    {
        nodes_pruned++;       
    }
    else
    {
        // Move ordering and cache check for pruning
        int indexes[nodes_to_explore] = {};
        float heuristic_scores[nodes_to_explore] = {};
        for(int i = 0, size = 0; size < nodes_to_explore; i++)
        {
            if(!is_id_cached(node->children[i]->id))
            {
                int pos = size;
                float fast_score = get_heuristic_score(node->children[i], 10);
                for(; pos > 0 && heuristic_scores[pos - 1] > fast_score; pos--)
                {   
                    indexes[pos] = indexes[pos - 1];
                    heuristic_scores[pos] = heuristic_scores[pos - 1];
                }
                indexes[pos] = i;
                heuristic_scores[pos] = fast_score;
                size++;
            }
        }

        // Search for the children node with the minimum score
        for (int i = 0; i < nodes_to_explore; i++)
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

float PMiniMaxPlayer::get_heuristic_score(Node *node, int rollouts)
{
    float score = 0;

    #pragma omp parallel for reduction(+:score)
    for (int i = 0; i < rollouts; i++)
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

    return score / rollouts;
}
