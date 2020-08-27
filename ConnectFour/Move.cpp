#include "Move.h"
#include <iostream>

namespace ConnectFour
{
    Move::Move(int column)
    {
        this->column = column;
    }

    std::string Move::to_string()
    {
        std::string a = "A";
        a[0] += column;
        return a;
    }

    Engine::IMove *Move::get_copy()
    {
        return new Move(column);
    }
} // namespace ConnectFour
