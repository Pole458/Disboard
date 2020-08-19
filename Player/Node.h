#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/IMove.h"

class Node
{
    public:
        Node();
        ~Node();

        float played = 0;
        float score = 0;

        IBoard* board;

        IMove* move;

        Node* parent;
        std::vector<Node*> children;

        float get_ucb(int total_played);
        float get_inverse_ucb(int total_played);
        float get_winrate();
};