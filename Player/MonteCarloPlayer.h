#pragma once

#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "../Pcg/pcg_random.hpp"
#include "Node.h"
#include "Score.h"

#include <unordered_map>

class MonteCarloPlayer : public IPlayer
{

public:
    MonteCarloPlayer(float time_to_think, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:
    // Random generator
    pcg32 rng;

    // Internal random player used to for random rollouts
    RandomPlayer player;

    float thinking_time;

    bool verbose;
};