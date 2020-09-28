#pragma once

#include "BoardId.h"
#include "IBoard.h"
#include "IPlayer.h"

namespace Engine
{
    // Plays a game between p1 and p2 starting from the board status until the game is over. p1 goes first.
    void play(IBoard *board, IPlayer *p1, IPlayer *p2, bool verbose = false);
};