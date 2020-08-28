#pragma once

#include "../Engine/IPlayer.h"

#include <vector>
#include <iostream>

// Class for a Random player

class RandomPlayer : public IPlayer
{
	public:
		RandomPlayer();
		~RandomPlayer();

		Engine::IMove* choose_move(Engine::IBoard* board);
};