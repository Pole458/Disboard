#pragma once

#include "../Engine/IPossibleMoves.h"
#include "ConnectFour.h"
#include "Board.h"

namespace ConnectFour
{
    class PossibleMoves : public Engine::IPossibleMoves
    {
        public:
            PossibleMoves(Board* board);
            ~PossibleMoves();

            BitBoard bitboard;
            
            int size();
            Engine::IMove* move_at(int move_index);
            std::string to_string();

        private:
            int _size; // cached value;
    };
}