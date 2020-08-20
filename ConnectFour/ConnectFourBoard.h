#pragma once

#include "../Engine/IBoard.h"
#include "../Engine/Game.h"

#include "ConnectFourMove.h"

#include <string>
#include <vector>

#define WIDTH 7
#define HEIGHT 6

class ConnectFourBoard : public IBoard
{

    public:

        static const char space = '*';
        static const char red = 'X';
        static const char yellow = 'O';

        ConnectFourBoard();
        ~ConnectFourBoard();

        void play(IPlayer* p1, IPlayer* p2, bool verbose = false);

        std::vector<IMove*> get_possible_moves();
        void make_move(IMove* move);
        bool check_victory(IMove* last_move);

        void reset();

        bool should_keep_going();

        std::string to_string();

        IBoard* get_copy();

        void copy(IBoard* board_to_copy);

    private:
        char board[HEIGHT][WIDTH];

};