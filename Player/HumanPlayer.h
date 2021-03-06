#pragma once

#include "../Engine/IPlayer.h"

class HumanPlayer : public IPlayer
{
    public:
		HumanPlayer();
		
		Engine::IMove* choose_move(Engine::IBoard* board);
};