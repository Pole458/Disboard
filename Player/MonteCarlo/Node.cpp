#include "Node.h"

#include <iostream>

Node::Node(Engine::IBoard* board, Engine::IMove* move, Node* parent)
{
    this->board = board;
    this->move = move;
    this->parent = parent;

    is_leaf = true;
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
    // std::cout << "Deleting node " << id << std::endl;

    delete board;
    delete move;
    // std::cout << "    Deleted move" << std::endl;
    
    if(expanded)
    {
        for(int i = 0; i < possible_moves->size(); i++)
        {
            delete children[i];
        }
        // std::cout << "    Deleting children" << std::endl;
        
        delete[] children;
        // std::cout << "    Deleting children array" << std::endl;
    }

    delete possible_moves;
    // std::cout << "    Deleting possible moves" << std::endl;
}