#include "MiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <iostream>

MiniMaxPlayer::MiniMaxPlayer(int depth, bool verbose)
{
    this->max_depth = depth;
    this->verbose = verbose;
}

Engine::IMove *MiniMaxPlayer::choose_move(Engine::IBoard *board)
{

    // Reset analytcs values
    nodes_evaluated = 0;
    pruned = 0;
    cached = 0;

    int highscore = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    Node root(board->get_copy());
    root.expand();
    Engine::IMove *selected_move = NULL;
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
        }
    }

    if (verbose)
    {
        std::cout << "Nodes visited " << nodes_evaluated << std::endl
                  << "Pruned: " << pruned << std::endl
                  << "Cache hits " << cached << std::endl;
    }

    // Clear caches
    scores.clear();
    evaluated.clear();

    return selected_move->get_copy();
}

int MiniMaxPlayer::maximize(Node *node, int depth, int alpha, int beta)
{

    // If this board position has already been evaluated, return the cached value
    if (evaluated[node->id])
    {
        cached++;
        return scores[node->id];
    }

    nodes_evaluated++;

    if (depth == 0 || node->is_leaf())
    {
        // If node is terminal or max depth was reached, evaluate board configuration
        scores[node->id] = -node->board->get_score();
        evaluated[node->id] = true;
        return scores[node->id];
    }

    node->expand();
    int highscore = std::numeric_limits<int>::min();
    // Search for the children node with the maximum score
    for (int i = 0; i < node->possible_moves->size(); i++)
    {
        highscore = std::max(highscore, minimize(node->children[i], depth - 1, alpha, beta));

        // Prune search
        alpha = std::max(alpha, highscore);
        if (beta <= alpha)
        {
            pruned++;
            break;
        }
    }
    node->reduce();

    scores[node->id] = highscore;
    evaluated[node->id] = true;

    return highscore;
}

int MiniMaxPlayer::minimize(Node *node, int depth, int alpha, int beta)
{

    // If this board position has already been evaluated, return the cached value
    if (evaluated[node->id])
    {
        cached++;
        return scores[node->id];
    }

    if (depth == 0 || node->is_leaf())
    {
        // If node is terminal or max depth was reached, evaluate board configuration
        scores[node->id] = node->board->get_score();
        evaluated[node->id] = true;
        return scores[node->id];
    }

    nodes_evaluated++;

    node->expand();
    int lowscore = std::numeric_limits<int>::max();
    // Search for the children node with the minimum score
    for (int i = 0; i < node->possible_moves->size(); i++)
    {
        lowscore = std::min(lowscore, maximize(node->children[i], depth - 1, alpha, beta));

        // Prune search
        beta = std::min(beta, lowscore);
        if (beta <= alpha)
        {
            pruned++;
            break;
        }
    }
    node->reduce();

    scores[node->id] = lowscore;
    evaluated[node->id] = true;

    return lowscore;
}