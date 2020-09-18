#pragma once

#include "../Engine/Engine.h"

class Score
{
public:
    Score();
    Score(const Score &score);
    Score(float score, float played);

    float played;
    float score;

    float get_ucb(int total_played);
    float get_inverse_ucb(int total_played);
    float get_winrate();
    void increase(float score, float played);
    void increase(Score score);
};