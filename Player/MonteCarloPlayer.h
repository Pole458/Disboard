#pragma once

#include "IPlayer.h"
#include "RandomPlayer.h"
#include "../Engine/Game.h"
#include "Node.h"

class MonteCarloPlayer : public IPlayer
{
        
    public:
        MonteCarloPlayer(int rollouts, bool verbose);
        ~MonteCarloPlayer();

        Engine::IMove* choose_move(Engine::IBoard* board);

    private:
        RandomPlayer* player;
        int _rollouts;
        bool _verbose;
};