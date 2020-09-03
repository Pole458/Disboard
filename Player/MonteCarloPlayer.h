#pragma once

#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>

class MonteCarloPlayer : public IPlayer
{

public:
    MonteCarloPlayer(int rollouts, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:
    // Internal random player used to for random rollout
    RandomPlayer player;

    // Number to rollout needed to choose a move
    int rollouts;

    bool verbose;
};