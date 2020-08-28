#pragma once

#include "BoardId.h"
#include "IBoard.h"
#include "IPlayer.h"

namespace Engine
{
    void play(IBoard *board, IPlayer *p1, IPlayer *p2, bool verbose = false);
};