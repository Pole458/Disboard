#pragma once

#include "../Engine/Engine.h"

class Score
{
public:
    Score();

    float played;
    float score;

    float get_ucb(int total_played);
    float get_inverse_ucb(int total_played);
    float get_winrate();
    void increase(float score, float played);
};