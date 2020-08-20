#pragma once

#include "IPlayer.h"

class HumanPlayer : public IPlayer
{
    public:
		HumanPlayer();
		~HumanPlayer();

		IMove* choose_move(IBoard* board);
};