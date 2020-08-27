#include "PossibleMoves.h"

#include <iostream>

namespace ConnectFour
{
   
    PossibleMoves::PossibleMoves(Board *board)
    {

        bitboard = (~board->both) & upper_row_mask;

        if (board->status != Engine::IBoard::Ongoing)
        {
            _size = 0;
        }
        else
        {
            _size = count_bits(bitboard);
        }
    }

    PossibleMoves::~PossibleMoves()
    {
    }

    int PossibleMoves::size()
    {
        return _size;
    }

    Engine::IMove *PossibleMoves::move_at(int move_index)
    {
        BitBoard mask = 1UL;
        for(; move_index > 0 || !(bitboard & mask); mask = mask << 1)
        {
            if(bitboard & mask)
            {
                move_index--;
            }
        }

        return new Move(mask);
    }

    std::string PossibleMoves::to_string()
    {
        std::string s = "";

        int c = 0;
        for (BitBoard mask = 1UL; mask & upper_row_mask; mask = mask << 1)
        {
            if (bitboard & mask)
            {
                s += ('0' + c);
                c++;
            }
            else s += "-";

            s += " ";
        }

        return s;
    }
} // namespace ConnectFour