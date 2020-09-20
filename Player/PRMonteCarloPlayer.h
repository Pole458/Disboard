#pragma once

#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>
#include <omp.h>

class PRMonteCarloPlayer : public IPlayer
{

public:
    PRMonteCarloPlayer(float thinking_time, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:

    // Internal random player used to for random rollout
    RandomPlayer player;

    // Number to rollout needed to choose a move
    float thinking_time;

    bool verbose;
};