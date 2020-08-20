#pragma once

#include "IMove.h"

#include <string>
#include <vector>

// Interface for a Board. Stores the state of the game

class IBoard
{

    public:

        enum Status {Ongoing, Draw, First, Second};

        int turn = 1;

        Status status;

        virtual std::vector<IMove*> get_possible_moves() = 0;
        virtual void make_move(IMove* move) = 0;
        virtual bool check_victory(IMove* last_move) = 0;

        virtual void reset() = 0;

        virtual std::string to_string() = 0;

        virtual bool should_keep_going() = 0;

        virtual IBoard* get_copy() = 0;

        virtual void copy(IBoard* board_to_copy) = 0;
};