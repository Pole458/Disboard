#pragma once

#include "../../Engine/IBoard.h"

class Node
{
public:
    Node(Engine::IBoard *board, Engine::IMove *move = NULL, Node *parent = NULL);
    ~Node();

    Engine::IBoard *board;
    Engine::IMove *move;

    bool is_leaf;
    bool expanded;

    // Cached values
    Engine::board_id id;
    Engine::IPossibleMoves *possible_moves;

    Node* parent;
    Node** children;
};