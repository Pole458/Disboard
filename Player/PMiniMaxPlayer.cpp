#include "PMiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <iostream>
#include <chrono>

PMiniMaxPlayer::PMiniMaxPlayer(int depth, bool verbose)
{
    this->max_depth = depth;
    this->verbose = verbose;

    omp_init_lock(&lock);
}

Engine::IMove *PMiniMaxPlayer::choose_move(Engine::IBoard *board)
{

    // Reset analytcs values
    nodes_evaluated = 0;
    pruned = 0;
    cached = 0;

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    int highscore = std::numeric_limits<int>::min();

    Node root(board->get_copy());
    root.expand();
    Engine::IMove *selected_move = NULL;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Search for the children node with the maximum score
    for (int i = 0; i < root.possible_moves->size(); i++)
    {
        int score = minimize(root.children[i], max_depth - 1, alpha, beta);

        if (verbose)
            std::cout << root.children[i]->move->to_string() << " score: " << score << std::endl;

        if (score > highscore)
        {
            highscore = score;
            selected_move = root.children[i]->move;
            alpha = std::max(alpha, highscore);
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if (verbose)
    {
        std::cout << "Nodes visited " << nodes_evaluated << std::endl
                  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl
                  << "Pruned: " << pruned << std::endl
                  << "Cache hits " << cached << std::endl;
    }

    // Clear caches
    scores.clear();

    return selected_move->get_copy();
}

int PMiniMaxPlayer::maximize(Node *node, int depth, int alpha, int beta)
{

    // std::cout << "max, depth " << depth << std::endl;

    // If this board position has already been evaluated, return the cached value
    if (scores.find(node->id) != scores.end())
    {
        cached++;
        return scores.at(node->id);
    }

    nodes_evaluated++;

    if (depth == 0 || node->is_leaf())
    {
        // If node is terminal or max depth was reached, evaluate board configuration
        int score = -node->board->get_score();
        omp_set_lock(&lock);
        scores.emplace(node->id, score); 
        omp_unset_lock(&lock);
        return score;
    }

    node->expand();
    // Search for the children node with the maximum score

    int highscore = minimize(node->children[0], depth - 1, alpha, beta);
    alpha = std::max(alpha, highscore);
    if (beta <= alpha)
    {
        pruned++;
    }
    else
    {
        omp_lock_t prune_lock;
        omp_init_lock(&prune_lock);

        bool prnd = false;

#pragma omp parallel for
        for (int i = 1; i < node->possible_moves->size(); i++)
        {
            omp_set_lock(&prune_lock);

            if (!prnd)
            {
                omp_unset_lock(&prune_lock);

                int score = pminimize(node->children[i], depth - 1, alpha, beta);

                omp_set_lock(&prune_lock);

                // Prune search
                highscore = std::max(highscore, score);
                alpha = std::max(alpha, highscore);
                if (beta <= alpha)
                {
                    pruned++;
                    prnd = true;
                }
            }

            omp_unset_lock(&prune_lock);
        }
    }

    node->reduce();

    omp_set_lock(&lock);
    scores.emplace(node->id, highscore);
    omp_unset_lock(&lock);

    return highscore;
}

int PMiniMaxPlayer::minimize(Node *node, int depth, int alpha, int beta)
{
    // std::cout << "min, depth " << depth << std::endl;

    // If this board position has already been evaluated, return the cached value
    if (scores.find(node->id) != scores.end())
    {
        cached++;
        return scores.at(node->id);
    }

    if (depth == 0 || node->is_leaf())
    {
        // If node is terminal or max depth was reached, evaluate board configuration
        int score = node->board->get_score();
        omp_set_lock(&lock);
        scores.emplace(node->id, score);
        omp_unset_lock(&lock);
        return score;
    }

    nodes_evaluated++;

    node->expand();
    int lowscore = maximize(node->children[0], depth - 1, alpha, beta);
    beta = std::min(beta, lowscore);
    if (beta <= alpha)
    {
        pruned++;
    }
    else
    {
        omp_lock_t prune_lock;
        omp_init_lock(&prune_lock);

        bool prnd = false;

// Search for the children node with the minimum score
#pragma omp parallel for
        for (int i = 1; i < node->possible_moves->size(); i++)
        {
            omp_set_lock(&prune_lock);

            if (!prnd)
            {
                omp_unset_lock(&prune_lock);

                int score = pmaximize(node->children[i], depth - 1, alpha, beta);

                omp_set_lock(&prune_lock);

                lowscore = std::min(lowscore, score);
                beta = std::min(beta, lowscore);
                if (beta <= alpha)
                {
                    pruned++;
                    prnd = true;
                }
            }

            omp_unset_lock(&prune_lock);
        }
    }

    node->reduce();

    omp_set_lock(&lock);
    scores.emplace(node->id, lowscore);
    omp_unset_lock(&lock);

    return lowscore;
}

int PMiniMaxPlayer::pmaximize(Node *node, int depth, int alpha, int beta)
{

    // If this board position has already been evaluated, return the cached value
    if (scores.find(node->id) != scores.end())
    {
        cached++;
        return scores.at(node->id);
    }

    nodes_evaluated++;

    if (depth == 0 || node->is_leaf())
    {
        // If node is terminal or max depth was reached, evaluate board configuration
        int score = -node->board->get_score();
        omp_set_lock(&lock);
        scores.emplace(node->id, score);
        omp_unset_lock(&lock);
        return score;
    }

    node->expand();
    int highscore = std::numeric_limits<int>::min();
    // Search for the children node with the maximum score
    for (int i = 0; i < node->possible_moves->size(); i++)
    {
        highscore = std::max(highscore, pminimize(node->children[i], depth - 1, alpha, beta));

        // Prune search
        alpha = std::max(alpha, highscore);
        if (beta <= alpha)
        {
            pruned++;
            break;
        }
    }
    node->reduce();

    omp_set_lock(&lock);
    scores.emplace(node->id, highscore);
    omp_unset_lock(&lock);
    
    return highscore;
}

int PMiniMaxPlayer::pminimize(Node *node, int depth, int alpha, int beta)
{

    // If this board position has already been evaluated, return the cached value
    if (scores.find(node->id) != scores.end())
    {
        cached++;
        return scores.at(node->id);
    }

    nodes_evaluated++;

    if (depth == 0 || node->is_leaf())
    {
        // If node is terminal or max depth was reached, evaluate board configuration
        int score = node->board->get_score();
        omp_set_lock(&lock);
        scores.emplace(node->id, score);
        omp_unset_lock(&lock);
        
        return score;
    }

    node->expand();
    int lowscore = std::numeric_limits<int>::max();
    // Search for the children node with the minimum score
    for (int i = 0; i < node->possible_moves->size(); i++)
    {
        lowscore = std::min(lowscore, pmaximize(node->children[i], depth - 1, alpha, beta));

        // Prune search
        beta = std::min(beta, lowscore);
        if (beta <= alpha)
        {
            pruned++;
            break;
        }
    }
    node->reduce();

    omp_set_lock(&lock);
    scores.emplace(node->id, lowscore);
    omp_unset_lock(&lock);

    return lowscore;
}