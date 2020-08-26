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

        Engine::IMove* choose_move(Engine::IBoard* board);

    private:
        RandomPlayer* player;
};