#include "Node.h"

#include "math.h"
#include <iostream>
#include <limits>

Node::Node(Engine::IBoard* board, Engine::IMove* move, Node* parent)
{
    this->board = board;
    this->move = move;
    this->parent = parent;

    played = 0;
    score = 0;
}

Node::~Node()
{
    delete board;
    delete move;

    for(auto child : children)
    {
        delete child;
    }
    children.clear();
}

float Node::get_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return score / played + 2 * sqrt(log(total_played) / played);
}

float Node::get_inverse_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return (played - score) / played + 2 * sqrt(log(total_played) / played);
}

float Node::get_winrate()
{
    if(played == 0) return std::numeric_limits<float>::max();
    return score / played;
}

float Node::get_inverse_winrate()
{
    if(played == 0) return std::numeric_limits<float>::max();
    return (played - score) / played;
}