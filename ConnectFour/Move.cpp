#include "Move.h"

namespace ConnectFour
{
    ConnectFourMove::ConnectFourMove(int column, BitBoard c)
    {
        this->column = column;
        this->bit_column = c;
    }

    std::string ConnectFourMove::to_string()
    {
        std::string a = "A";
        a[0] += column;
        return a;
    }

    Engine::IMove *ConnectFourMove::get_copy()
    {
        return new ConnectFourMove(column, bit_column);
    }
} // namespace ConnectFour
