#include "PScore.h"

#include "math.h"

#include <limits>

PScore::PScore()
{
    played = 0;
    score = 0;

    virtual_loss = false;

    omp_init_lock(&lock);
}

float PScore::get_ucb(int total_played)
{
    omp_set_lock(&lock);

    float ucb;

    if(virtual_loss) ucb = 0;
    else if(played == 0) ucb = std::numeric_limits<float>::max();
    else ucb = score / played + 2 * sqrt(log(total_played) / played);

    omp_unset_lock(&lock);

    return ucb;
}

float PScore::get_inverse_ucb(int total_played)
{

    omp_set_lock(&lock);
    float ucb;

    if(virtual_loss) ucb = 0;
    else if(played == 0) ucb = std::numeric_limits<float>::max();
    else ucb = (played - score) / played + 2 * sqrt(log(total_played) / played);

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

void  PScore::increase(float score, float played)
{
    omp_set_lock(&lock);

    this->score += score;
    this->played += played;

    omp_unset_lock(&lock);
}

float PScore::get_winrate()
{
    omp_set_lock(&lock);

    float winrate;
    if(virtual_loss) winrate = 0;
    else if(played == 0) winrate = std::numeric_limits<float>::max();
    else winrate = score / played;

    omp_unset_lock(&lock);

    return winrate;
}

void PScore::set_virtual_loss(bool b)
{
    omp_set_lock(&lock);

    virtual_loss = b;

    omp_unset_lock(&lock);
}