#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/IMove.h"

class Node
{
    public:
        Node(IBoard* board, IMove* move = NULL, Node* parent = NULL);
        ~Node();

        float played;
        float score;

        IBoard* board;
        IMove* move;

        Node* parent;
        std::vector<Node*> children;

        float get_ucb(int total_played);
        float get_inverse_ucb(int total_played);
        float get_winrate();
        float get_inverse_winrate();
};