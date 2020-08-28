#include "Node.h"

Node::Node(Engine::IBoard* board, Engine::IMove* move, Node* parent)
{
    this->board = board;
    this->move = move;
    this->parent = parent;

    is_leaf = true;

    // Apply move
    if(move != NULL)
        board->make_move(move);

    // Caching values
    id = board->get_id();
    possible_moves = board->get_possible_moves();
}

Node::~Node()
{
    delete move;
    
    delete[] children;
    delete possible_moves;
}