#pragma once

#include "../Engine/IPossibleMoves.h"
#include "fwd.h"
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

        private:
            int _size; // cached value;
    };
}