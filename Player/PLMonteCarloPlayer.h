#pragma once


#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "Node.h"
#include "Score.h"

#include <omp.h>


class PLMonteCarloPlayer : public IPlayer
{

public:
    PLMonteCarloPlayer(float thinking_time, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:

    // Internal random player used to for random rollout
    RandomPlayer player;

    float thinking_time;

    bool verbose;
};