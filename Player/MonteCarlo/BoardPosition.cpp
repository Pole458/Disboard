#include "BoardPosition.h"

#include "math.h"
#include <limits>

BoardPosition::BoardPosition()
{
    id = 0UL;
    played = 0;
    score = 0;
}

BoardPosition::BoardPosition(Engine::board_id board_id)
{
    id = board_id;
    played = 0;
    score = 0;
}

float BoardPosition::get_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return score / played + 2 * sqrt(log(total_played) / played);
}

float BoardPosition::get_inverse_ucb(int total_played)
{
    if(played == 0) return std::numeric_limits<float>::max();
    return (played - score) / played + 2 * sqrt(log(total_played) / played);
}

float BoardPosition::get_winrate()
{
    if(played == 0) return std::numeric_limits<float>::max();
    return score / played;
}

float BoardPosition::get_inverse_winrate()
{
    if(played == 0) return std::numeric_limits<float>::max();
    return (played - score) / played;
}