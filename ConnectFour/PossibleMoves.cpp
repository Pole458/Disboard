#include "PossibleMoves.h"

namespace ConnectFour
{
   
    PossibleMoves::PossibleMoves(Board *board)
    {
        if (board->status != Engine::IBoard::Ongoing)
        {
            _size = 0;
        }
        else
        {
            for (int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
            {
                if (bitboard & (2 ^ columnIndex))
                {
                    _size++;
                }
            }
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
        int column_index = 0;
        while (column_index < move_index)
        {
            if (bitboard & (2 ^ column_index))
            {
                column_index++;
            }
        }

        return new ConnectFourMove(column_index, (BitBoard)(2 ^ column_index));
    }
} // namespace ConnectFour