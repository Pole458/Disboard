#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/Game.h"

#include "Move.h"
#include "PossibleMoves.h"

#include <string>
#include <vector>

#define WIDTH 7
#define HEIGHT 6

namespace ConnectFour
{

    class Board : public Engine::IBoard
    {

    public:
        static const char space = '*';
        static const char red = 'X';
        static const char yellow = 'O';

        Board();
        ~Board();

        void play(IPlayer *p1, IPlayer *p2, bool verbose = false);

        Engine::IPossibleMoves *get_possible_moves();
        void make_move(Engine::IMove *move);
        bool check_victory(Engine::IMove *last_move);

        void reset();

        bool should_keep_going();

        std::string to_string();

        IBoard *get_copy();

        void copy(IBoard *board_to_copy);

    private:
        char board[HEIGHT][WIDTH];
        BitBoard both;
        BitBoard current;

        static const BitBoard upper_row = 2 ^ (WIDTH - 1); // 2 ^ 6 = 64 = 0x111111
    };
} // namespace ConnectFour
