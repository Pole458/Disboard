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

    void increase(float wins, float played);
    void increase(PScore score);

private:
    omp_lock_t lock;

    float wins;
    int played;
};