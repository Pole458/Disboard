#include "PNode.h"

PNode::PNode(Engine::IBoard *board, Engine::IMove *move, PNode *parent)
{
    this->board = board;
    this->move = move;
    this->parent = parent;

    expanded = false;
    explored = false;

    // Apply move
    if(move != NULL)
        board->make_move(move);

    children = NULL;

    // Caching values
    id = board->get_id();
    possible_moves = board->get_possible_moves();

    omp_init_lock(&lock);
}

PNode::~PNode()
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

    omp_destroy_lock(&lock);
}

bool PNode::is_leaf()
{
    return possible_moves->size() == 0;
}

void PNode::expand()
{
    omp_set_lock(&lock);

    if (!expanded && possible_moves->size() > 0)
    {
        children = new PNode*[possible_moves->size()];
        for (int i = 0; i < possible_moves->size(); i++)
        {
            PNode *child = new PNode(board->get_copy(), possible_moves->move_at(i), this);
            children[i] = child;
        }
        expanded = true;
    }

    omp_unset_lock(&lock);
}

void PNode::reduce()
{
    omp_set_lock(&lock);

    if(expanded)
    {
        for(int i = 0; i < possible_moves->size(); i++)
        {
            delete children[i];
        }        
        delete[] children;

        expanded = false;
    }

    omp_unset_lock(&lock);
}

bool PNode::is_expanded()
{
    omp_set_lock(&lock);

    bool b = expanded;

    omp_unset_lock(&lock);

    return b;
}

void PNode::set_explored(bool b)
{
    omp_set_lock(&lock);

    explored = b;

    omp_unset_lock(&lock);
}

bool PNode::is_being_explored()
{
    omp_set_lock(&lock);

    bool b = explored;

    omp_unset_lock(&lock);

    return b;
}