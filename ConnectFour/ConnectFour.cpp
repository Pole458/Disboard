#include "ConnectFour.h"

#include <iostream>

namespace ConnectFour
{
    std::string bitboard_to_string(BitBoard bitboard)
    {

        std::string s = "A B C D E F G \n";

        for (int row_index = 0; row_index < HEIGHT - 1; row_index++)
        {
            for (int column_index = 0; column_index < WIDTH; column_index++)
            {
                if (bitboard & mask_at(row_index, column_index))
                    s += '1';
                else
                    s += '0';

                s += ' ';
            }
            s += '\n';
        }

        return s;
    }

    int count_bits(BitBoard bitboard)
    {
        int count = 0;
        while (bitboard > 0)
        {
            bitboard &= (bitboard - 1);
            count++;
        }
        return count;
    }

    BitBoard top_mask(int column)
    {
        return (1UL << (HEIGHT - 1)) << (column * HEIGHT);
    }

    BitBoard bottom_mask(int column)
    {
        return 1UL << (column * HEIGHT);
    }

    BitBoard mask_at(int row, int column)
    {
        return (1UL << row) << (column * HEIGHT);
    }

    bool check_victory(BitBoard bitboard)
    {
        // horizontal
        BitBoard mask = bitboard & (bitboard >> (HEIGHT + 1));
        if (mask & (mask >> (2 * (HEIGHT + 1))))
            return true;

        // diagonal 1
        mask = bitboard & (bitboard >> HEIGHT);
        if (mask & (mask >> (2 * HEIGHT)))
            return true;

        // diagonal 2
        mask = bitboard & (bitboard >> (HEIGHT + 2));
        if (mask & (mask >> (2 * (HEIGHT + 2))))
            return true;

        // vertical
        mask = bitboard & (bitboard >> 1);
        if (mask & (mask >> 2))
            return true;

        return false;
    }
} // namespace ConnectFour