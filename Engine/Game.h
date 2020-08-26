#pragma once

#include <vector>

#include "IBoard.h"
#include "../Player/IPlayer.h"

namespace Engine
{

    void play(IBoard *board, IPlayer *p1, IPlayer *p2, bool verbose = false);

};