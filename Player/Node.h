#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/IMove.h"

class Node
{
    public:
        Node();
        ~Node();

        float score = 0;

        IBoard* board;

        IMove* move;

        Node* parent;
        std::vector<Node*> children;
};