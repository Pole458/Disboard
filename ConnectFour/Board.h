#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/Game.h"

#include "ConnectFour.h"
#include "Move.h"
#include "PossibleMoves.h"

#include <string>
#include <vector>

namespace ConnectFour
{

    class Board : public Engine::IBoard
    {

    public:
        Board();
        ~Board();

        void play(IPlayer *p1, IPlayer *p2, bool verbose = false);

        Engine::IPossibleMoves *get_possible_moves();
        void make_move(Engine::IMove *move);

        void reset();

        bool should_keep_going();

        std::string to_string();

        IBoard *get_copy();

        void copy(IBoard *board_to_copy);

        BitBoard both;
        BitBoard current;
    };
} // namespace ConnectFour
