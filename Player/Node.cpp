#include "Node.h"

#include "math.h"
#include <iostream>
#include <limits>

Node::Node()
{
    
}

Node::~Node()
{

    delete move;

    for(auto c : children)
    {
        delete c;
    }
    children.clear();
}

float Node::get_ucb(int total_played)
{
    if(played == 0)
        return std::numeric_limits<float>::max();
    // std::cout << "score: " << score << " total_played: " << total_played << " played: " << played << std::endl;
    return score / played + 2 * sqrt(log(total_played) / played);
}

float Node::get_inverse_ucb(int total_played)
{
    if(played == 0)
        return std::numeric_limits<float>::max();

    return (played - score) / played + 2 * sqrt(log(total_played) / played);
}

float Node::get_winrate()
{
    return score / played;
}