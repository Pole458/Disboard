#pragma once

#include <string>

// Interface for a Move

class IMove {

    public:

        virtual std::string to_string() = 0;
};