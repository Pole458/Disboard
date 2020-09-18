#pragma once

#include "../Engine/IBoard.h"

#include <omp.h>

class PNode
{
public:
    PNode(Engine::IBoard *board, Engine::IMove *move = NULL, PNode *parent = NULL);
    ~PNode();

    Engine::IBoard *board;
    Engine::IMove *move;

    void expand();
    void reduce();
    bool is_leaf();

    bool is_expanded();

    void set_explored(bool b);
    bool is_being_explored();

    PNode* parent;
    PNode** children;

    // Cached values
    Engine::board_id id;
    Engine::IPossibleMoves *possible_moves;

private:
    omp_lock_t lock;
    omp_lock_t explored_lock;

    bool expanded;
    bool explored;
};