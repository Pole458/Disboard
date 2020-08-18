#pragma once

#include "../Engine/IMove.h"

#include <string>

class ConnectFourMove : public IMove
{
    public:
        ConnectFourMove(int column);
        
        int column;
        
        std::string to_string();
};