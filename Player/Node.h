#pragma once

#include "../Engine/IBoard.h"

class Node
{
public:
    Node(Engine::IBoard *board, Engine::IMove *move = NULL, Node *parent = NULL);
    ~Node();

    void expand();
    void reduce();
    bool is_leaf();

    Engine::IBoard *board;
    Engine::IMove *move;

    bool expanded;

    // Cached values
    Engine::board_id id;
    Engine::IPossibleMoves *possible_moves;

    Node* parent;
    Node** children;
};