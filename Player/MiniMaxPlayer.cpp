#include "MiniMaxPlayer.h"
#include "../Engine/IPossibleMoves.h"

#include <iostream>

MiniMaxPlayer::MiniMaxPlayer(int depth, bool verbose)
{
    _depth = depth;
    _verbose = verbose;
}

Engine::IMove* MiniMaxPlayer::choose_move(Engine::IBoard* board)
{

    scores.clear();

    Node root(board->get_copy());

    visited_nodes = 0;
    pruned = 0;
    reflected = 0;

    int highscore = std::numeric_limits<int>::min(); 
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    
    root.expand();
    Node *node = root.children[0];
    for(int i = 0; i < root.possible_moves->size(); i++)
    {
        int score = minimize(root.children[i], _depth - 1, alpha, beta); 

        if(_verbose)
            std::cout << root.children[i]->move->to_string() << " score: " << score << std::endl;

        if(score > highscore)
        {
            highscore = score;
            node = root.children[i];
        }        

        if(beta <= alpha)
        {
            pruned++;
            break;
        }
    }

    std::cout << "Nodes visited " << visited_nodes << std::endl;
    std::cout << "Pruned: " << pruned << std::endl;
    std::cout << "Reflected: " << reflected << std::endl;

    return node->move->get_copy();
}

int MiniMaxPlayer::maximize(Node* node, int depth, int alpha, int beta)
{

    if(depth == 0 || node->is_leaf())
    {
        if(scores[node->id] == 0)
        {
            scores[node->id] = -node->board->get_score();
        }
        else
        {
            reflected++;
        }

        return scores[node->id];
    }

    visited_nodes++;

    node->expand();
    int highscore = std::numeric_limits<int>::min();
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        highscore = std::max(highscore, minimize(node->children[i], depth - 1, alpha, beta));
        
        alpha = std::max(alpha, highscore);
        if(beta <= alpha)
        {
            pruned++;
            break;
        }
    }
    node->reduce();

    return highscore;
}

int MiniMaxPlayer::minimize(Node* node, int depth, int alpha, int beta)
{

    if(depth == 0 || node->is_leaf())
    {
        if(scores[node->id] == 0)
        {
            scores[node->id] = node->board->get_score();
        }
        else
        {
            reflected++;
        }

        return scores[node->id];
    }
    
    visited_nodes++;

    node->expand();
    int lowscore = std::numeric_limits<int>::max();
    for(int i = 0; i < node->possible_moves->size(); i++)
    {
        lowscore = std::min(lowscore, maximize(node->children[i], depth - 1, alpha, beta));
        
        beta = std::min(beta, lowscore);
        if(beta <= alpha)
        {
            pruned++;
            break;
        }
    }
    node->reduce();

    return lowscore;
}