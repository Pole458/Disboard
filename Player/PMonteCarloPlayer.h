#pragma once

#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "PNode.h"
#include "PScore.h"

#include <unordered_map>
#include <omp.h>

class PMonteCarloPlayer : public IPlayer
{

public:
    PMonteCarloPlayer(int rollouts, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:
    // Internal random player used to for random rollout
    RandomPlayer player;

    // Number to rollout needed to choose a move
    int rollouts;

    bool verbose;
};