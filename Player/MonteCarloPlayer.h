#pragma once

#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "../Engine/IPossibleMoves.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>
#include <unordered_set>
#include <chrono>

class MonteCarloPlayer : public IPlayer
{

public:
    MonteCarloPlayer(float time_to_think, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

    // Analytics
    int max_depth_reached;
    int iterations;

protected:
    float thinking_time;
    bool verbose;
};