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

    void set_under_rollout(bool rollout);
    bool is_under_rollout();

    PNode* parent;
    PNode** children;

    // Cached values
    Engine::board_id id;
    Engine::IPossibleMoves *possible_moves;

private:
    omp_lock_t lock;
    bool expanded;

    bool under_rollout;
};