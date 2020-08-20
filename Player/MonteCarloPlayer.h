#pragma once

#include "IPlayer.h"
#include "RandomPlayer.h"
#include "../Engine/Game.h"
#include "Node.h"

class MonteCarloPlayer : public IPlayer
{
        
    public:
        MonteCarloPlayer();
        ~MonteCarloPlayer();

        IMove* choose_move(IBoard* board);

    private:
        RandomPlayer* player;
};