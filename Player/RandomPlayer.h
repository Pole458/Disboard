#pragma once

#include "IPlayer.h"

#include <vector>
#include <iostream>

// Class for a Random player

class RandomPlayer : public IPlayer
{
	public:
		RandomPlayer();
		~RandomPlayer();

		IMove* choose_move(IBoard* board);

	protected:
		std::vector<IMove*> possible_moves;
		void clear_possible_moves();
};