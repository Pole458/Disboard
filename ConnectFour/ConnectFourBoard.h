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

        std::vector<IMove*> get_possible_moves();
        void make_move(IMove* move);
        bool check_victory(IMove* last_move);

        void reset();

        std::string to_string();


    private:
        char board[HEIGHT][WIDTH];

};