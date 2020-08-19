#pragma once

#include "RandomPlayer.h"

class StudPlayer : public RandomPlayer
{
    public:
		StudPlayer();
		~StudPlayer();

		IMove* choose_move(IBoard* board);

        IMove* first_move;

};