#include "Score.h"

#include "math.h"
#include <limits>

Score::Score()
{
    wins = 0;
    played = 0;
}

Score::Score(const Score &score)
{
    wins = score.wins;
    played = score.played;
}

Score::Score(float wins, float played)
{
    this->wins = wins;
    this->played = played;
}

float Score::get_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return wins / played + 2 * sqrt(log(total_played) / played);
}

float Score::get_inverse_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return (played - wins) / played + 2 * sqrt(log(total_played) / played);
}

float Score::get_winrate()
{
    if(played == 0) return std::numeric_limits<float>::max();
    return wins / played;
}

void Score::increase(float score, float played)
{
    this->wins += score;
    this->played += played;
}

void Score::increase(Score score)
{
    played += score.played;
    wins += score.wins;
}