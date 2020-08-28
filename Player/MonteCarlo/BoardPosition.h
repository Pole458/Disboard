#pragma once

#include "../../Engine/Engine.h"

class BoardPosition
{
public:
    BoardPosition();
    BoardPosition(Engine::board_id id);

    Engine::board_id id;
    float played;
    float score;

    float get_ucb(int total_played);
    float get_inverse_ucb(int total_played);
    float get_winrate();
    float get_inverse_winrate();
};