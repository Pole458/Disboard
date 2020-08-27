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
        BitBoard mask = (1UL << (HEIGHT - 1)) << (HEIGHT * move_index);
        int columm_index = move_index;
        for(; !(bitboard & mask); mask <<= HEIGHT, columm_index++);

        // for(; move_index > 0 || !(bitboard & mask); mask <<= WIDTH)
        // {
        //     if(bitboard & mask)
        //     {
        //         move_index--;
        //     }
        // }

        return new Move(columm_index);
    }

    std::string PossibleMoves::to_string()
    {
        std::string s = "";

        for(int column_index = 0, count = 0; column_index < WIDTH; column_index++)
        {
            if (bitboard & top_mask(column_index))
            {
                s += ('0' + count);
                count++;
            }
            else
            {
                s += '-';
            }

            s += ' ';
        }

        return s;
    }
} // namespace ConnectFour