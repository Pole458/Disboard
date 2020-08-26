#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/IMove.h"


// Interface for a generic player

class IPlayer
{
    public:
        virtual Engine::IMove* choose_move(Engine::IBoard* board) = 0;
};