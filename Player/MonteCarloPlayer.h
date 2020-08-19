#pragma once

#include "IPlayer.h"
#include "StudPlayer.h"
#include "../Engine/Game.h"
#include "Node.h"

class MonteCarloPlayer : public IPlayer
{
        
    public:
        MonteCarloPlayer();
        ~MonteCarloPlayer();

        IMove* choose_move(IBoard* board);

    private:
        Game my_game;

        StudPlayer* p1;
        StudPlayer* p2;
};