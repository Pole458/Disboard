#include "Node.h"

#include <iostream>

Node::Node(Engine::IBoard* board, Engine::IMove* move, Node* parent)
{
    this->board = board;
    this->move = move;
    this->parent = parent;

    expanded = false;

    // Apply move
    if(move != NULL)
        board->make_move(move);

    children = NULL;

    // Caching values
    id = board->get_id();
    possible_moves = board->get_possible_moves();
}

Node::~Node()
{
    delete board;
    delete move;
    
    if(expanded)
    {
        for(int i = 0; i < possible_moves->size(); i++)
        {
            delete children[i];
        }        
        delete[] children;
    }

    delete possible_moves;
}

bool Node::is_leaf()
{
    return board->get_possible_moves()->size() == 0;
}

void Node::expand()
{
    if (!expanded && possible_moves->size() > 0)
    {
        children = new Node*[possible_moves->size()];
        for (int i = 0; i < possible_moves->size(); i++)
        {
            Node *child = new Node(board->get_copy(), possible_moves->move_at(i), this);
            children[i] = child;
        }
        expanded = true;
    }
}

void Node::reduce()
{
    if(expanded)
    {
        for(int i = 0; i < possible_moves->size(); i++)
        {
            delete children[i];
        }        
        delete[] children;

        expanded = false;
    }
}