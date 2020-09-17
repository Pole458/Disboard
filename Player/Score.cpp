#include "Score.h"

#include "math.h"
#include <limits>

Score::Score()
{
    played = 0;
    score = 0;
}

float Score::get_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return score / played + 2 * sqrt(log(total_played) / played);
}

float Score::get_inverse_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return (played - score) / played + 2 * sqrt(log(total_played) / played);
}

float Score::get_winrate()
{
    if(played == 0) return std::numeric_limits<float>::max();
    return score / played;
}

void Score::increase(float score, float played)
{
    this->score += score;
    this->played += played;
}