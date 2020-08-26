#pragma once

#include "IMove.h"

#include <vector>

namespace Engine
{

    class IPossibleMoves
    {
    public:
        virtual int size() = 0;
        virtual IMove *move_at(int move_index) = 0;
    };
} // namespace Engine
