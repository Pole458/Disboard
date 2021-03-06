#include "MiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"


MiniMaxPlayer::MiniMaxPlayer(int depth, bool verbose, int cache_size)
{
    this->max_depth = depth;
    this->cache_size = cache_size;
    this->verbose = verbose;
}

Engine::IMove *MiniMaxPlayer::choose_move(Engine::IBoard *board)
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

float MiniMaxPlayer::maximize(Node *node, int depth, float alpha, float beta)
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

float MiniMaxPlayer::minimize(Node *node, int depth, float alpha, float beta)
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

bool MiniMaxPlayer::is_id_cached(Engine::board_id id)
{
    auto it = cached_id.find(id % cache_size);
    return it != cached_id.end() && it->second == id;
}

float MiniMaxPlayer::get_cached_score(Engine::board_id id)
{
    return cached_scores.at(id % cache_size);
}

void MiniMaxPlayer::set_cached_score(Engine::board_id id, float score)
{
    cached_id[id % cache_size] = id;
    cached_scores[id % cache_size] = score;
}

float MiniMaxPlayer::get_score(Node* node)
{
    if (node->board->status == Engine::IBoard::Draw)
    {
        // Draw
        return 0.5f;
    }
    else if ((my_turn == 1 && node->board->status == Engine::IBoard::First) || (my_turn == 0 && node->board->status == Engine::IBoard::Second))
    {
        // Win
        return 1 + 1.0f / node->board->turn;
    }

    // Loss
    return - 1.0f / node->board->turn;
}

float MiniMaxPlayer::get_heuristic_score(Node *node, int rollouts)
{
    float score = 0;;

    for (int i = 0; i < rollouts; i++)
    {
        Engine::IBoard *test_board = node->board->get_copy();
        Engine::play(test_board, &random_player, &random_player);

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
