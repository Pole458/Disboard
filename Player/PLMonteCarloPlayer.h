#pragma once

#include "MonteCarloPlayer.h"

#include <omp.h>

class PLMonteCarloPlayer : public MonteCarloPlayer
{

public:
    PLMonteCarloPlayer(float thinking_time, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);
};