#pragma once

#include "../Engine/IMove.h"
#include "fwd.h"

#include <string>

namespace ConnectFour
{

    class ConnectFourMove : public Engine::IMove
    {
    public:
        ConnectFourMove(int column, BitBoard c);

        int column;

        BitBoard bit_column;

        std::string to_string();

        Engine::IMove *get_copy();
    };
} // namespace ConnectFour