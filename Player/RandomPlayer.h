#pragma once

#include "../Engine/IPlayer.h"

#include "../Pcg/pcg_random.hpp"

#include <vector>
#include <random>

// Class for a Random player

class RandomPlayer : public IPlayer
{
	public:
		RandomPlayer();

		Engine::IMove* choose_move(Engine::IBoard* board);

		pcg32 rng;
};