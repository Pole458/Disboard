#pragma once

#include "../Engine/IPlayer.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>
#include <limits>

class MiniMaxPlayer : public IPlayer
{
public:
    MiniMaxPlayer(int depth, bool verbose = false);

    Engine::IMove* choose_move(Engine::IBoard* board);


private:
    int minimize(Node* node, int depth, int alpha, int beta);
    int maximize(Node* node, int depth, int alpha, int beta);

    bool _verbose;
    int _depth;
    int visited_nodes;
    int pruned;
    int reflected;

    std::unordered_map<Engine::board_id, int> scores;
};