#pragma once

#include "ConnectFour.h"
#include "../Engine/IMove.h"

namespace ConnectFour
{
    class Move : public Engine::IMove
    {
    public:
        Move(BitBoard c);

        BitBoard column;

        std::string to_string();

        Engine::IMove *get_copy();
    };
} // namespace ConnectFour