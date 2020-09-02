#pragma once

#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>

class MonteCarloPlayer : public IPlayer
{

public:
    MonteCarloPlayer(int rollouts, bool verbose = false, bool step_by_step = false);
    ~MonteCarloPlayer();

    Engine::IMove *choose_move(Engine::IBoard *board);

private:
    RandomPlayer *player;
    int _rollouts;
    bool _verbose;
    bool _step_by_step;
};