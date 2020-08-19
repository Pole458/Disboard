#pragma once

#include <vector>

#include "IBoard.h"
#include "IMove.h"

#include "../Player/IPlayer.h"

// Class for a Game,
// it needs a Board and two Players
// Allows to play a game and keeps track of moves history

class Game {

    public:

        Game();
        Game(IBoard* board, IPlayer* first, IPlayer* second);  
        ~Game();

        void play(bool verbose = false);

        IBoard* board;
        IPlayer* fist_player;
        IPlayer* second_player;
    
        std::vector<IMove*> move_history;
};