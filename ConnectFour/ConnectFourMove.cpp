#include "ConnectFourMove.h"

ConnectFourMove::ConnectFourMove(int column)
{
    this->column = column;
}
 
std::string ConnectFourMove::to_string()
{
    std::string a = "A";
    a[0] += column;
    return a;
}

IMove* ConnectFourMove::get_copy()
{
    return new ConnectFourMove(column);
}