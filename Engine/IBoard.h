#pragma once

#include "IMove.h"
#include "IPossibleMoves.h"

#include <string>
#include <vector>

// Interface for a Board. Stores the state of the game

namespace Engine
{

    class IBoard
    {

    public:
        enum Status
        {
            Ongoing,
            Draw,
            First,
            Second
        };

        int turn = 1;

        Status status;

        virtual IPossibleMoves *get_possible_moves() = 0;
        virtual void make_move(IMove *move) = 0;

        virtual void reset() = 0;

        virtual std::string to_string() = 0;

        virtual bool should_keep_going() = 0;

        virtual IBoard *get_copy() = 0;

        virtual void copy(IBoard *board_to_copy) = 0;
    };
} // namespace Engine