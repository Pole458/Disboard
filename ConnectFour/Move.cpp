#include "Move.h"
#include <iostream>

namespace ConnectFour
{
    Move::Move(BitBoard column)
    {
        this->column = column;
    }

    std::string Move::to_string()
    {
        std::string a = "A";
        int column_index = 0;
        // Find column index;
        for(BitBoard mask = 1; !(column & mask); mask = mask << 1, column_index++);

        a[0] += column_index;
        return a;
    }

    Engine::IMove *Move::get_copy()
    {
        return new Move(column);
    }
} // namespace ConnectFour
