#include "Node.h"

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