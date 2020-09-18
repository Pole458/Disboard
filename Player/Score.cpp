#include "Score.h"

#include "math.h"
#include <limits>

Score::Score()
{
    score = 0;
    played = 0;
}

Score::Score(const Score &score)
{
    this->score = score.score;
    this->played = score.played;
}

Score::Score(float score, float played)
{
    this->score = score;
    this->played = played;
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

void Score::increase(Score score)
{
    played += score.played;
    this->score += score.score;
}