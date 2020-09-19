#include "PScore.h"

#include "math.h"

#include <limits>

PScore::PScore()
{
    played = 0;
    wins = 0;
    
    omp_init_lock(&lock);
}

float PScore::get_ucb(int total_played)
{
    omp_set_lock(&lock);

    float ucb;

    if(played == 0) ucb = std::numeric_limits<float>::max();
    else ucb = wins / played + 2 * sqrt(log(total_played) / played);

    omp_unset_lock(&lock);

    return ucb;
}

float PScore::get_inverse_ucb(int total_played)
{

    omp_set_lock(&lock);
    float ucb;

    if(played == 0) ucb = std::numeric_limits<float>::max();
    else ucb = (played - wins) / played + 2 * sqrt(log(total_played) / played);

    omp_unset_lock(&lock);

    return ucb;
}

float PScore::get_played()
{
    omp_set_lock(&lock);

    float p = played;

    omp_unset_lock(&lock);

    return played;
}

void  PScore::increase(float wins, float played)
{
    omp_set_lock(&lock);

    this->wins += wins;
    this->played += played;

    omp_unset_lock(&lock);
}

float PScore::get_winrate()
{
    omp_set_lock(&lock);

    float winrate;
    if(played == 0) winrate = std::numeric_limits<float>::max();
    else winrate = wins / played;

    omp_unset_lock(&lock);

    return winrate;
}

void PScore::increase(PScore score)
{
    omp_set_lock(&lock);
    
    wins += score.wins;
    played += score.played;
    
    omp_unset_lock(&lock);
}