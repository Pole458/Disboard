#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/IMove.h"

// Interface for a generic player

class IPlayer
{
    public:
        virtual IMove* choose_move(IBoard* board) = 0;
};