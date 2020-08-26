#pragma once

#include <string>

namespace Engine
{

    // Interface for a Move
    class IMove
    {

    public:
        virtual std::string to_string() = 0;
        virtual IMove *get_copy() = 0;
    };
} // namespace Engine
