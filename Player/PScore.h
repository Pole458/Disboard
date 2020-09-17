#pragma once

#include "../Engine/Engine.h"

#include "omp.h"

class PScore
{
public:
    PScore();

    float get_ucb(int total_played);
    float get_inverse_ucb(int total_played);
    float get_played();
    float get_winrate();

    void increase(float score, float played);

private:
    omp_lock_t lock;

    float played;
    float score;
};