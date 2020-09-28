#pragma once

#include "MonteCarloPlayer.h"

#include <omp.h>

class PRMonteCarloPlayer : public MonteCarloPlayer
{

public:
    PRMonteCarloPlayer(float thinking_time, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);
};