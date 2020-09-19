#include "ConnectFour.h"

#include <iostream>

namespace ConnectFour
{
    std::string bitboard_to_string(BitBoard bitboard)
    {

        std::string s = "A B C D E F G \n";

        for (int row_index = HEIGHT - 2; row_index >= 0; row_index--)
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

    std::string complete_bitboard_to_string(BitBoard bitboard)
    {

        std::string s = "A B C D E F G \n";

        for (int row_index = HEIGHT - 1; row_index >= 0; row_index--)
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
        return (1UL << (HEIGHT - 2)) << (column * HEIGHT);
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
        // Horizontal -
        BitBoard mask = bitboard & (bitboard >> (HEIGHT));
        if (mask & (mask >> (2 * (HEIGHT))))
            return true;

        // Diagonal 
        mask = bitboard & (bitboard >> (HEIGHT - 1));
        if (mask & (mask >> (2 * (HEIGHT - 1))))
            return true;

        // Diagonal /
        mask = bitboard & (bitboard >> (HEIGHT + 1));
        if (mask & (mask >> (2 * (HEIGHT + 1))))
            return true;

        // Vertical |
        mask = bitboard & (bitboard >> 1);
        if (mask & (mask >> 2))
            return true;

        return false;
    }

    int get_heuristic_score(BitBoard bitboard)
    {
        int score = 0;

        // Check 4

        // Horizontal -
        BitBoard mask = bitboard & (bitboard >> (HEIGHT));
        mask = mask & (mask >> (2 * (HEIGHT)));
        score = count_bits(mask);
        if(score > 0) return 10000;

        // Diagonal 
        mask = bitboard & (bitboard >> (HEIGHT - 1));
        mask = mask & (mask >> (2 * (HEIGHT - 1)));
        score = count_bits(mask);
        if(score > 0) return 10000;

        // Diagonal /
        mask = bitboard & (bitboard >> (HEIGHT + 1));
        mask = mask & (mask >> (2 * (HEIGHT + 1)));
        score = count_bits(mask);
        if(score > 0) return 10000;

        // Vertical |
        mask = bitboard & (bitboard >> 1);
        mask = mask & (mask >> 2);
        score = count_bits(mask);
        if(score > 0) return 10000;


        // Check 3

        score = 0;

        // Horizontal -
        mask = bitboard & (bitboard >> (HEIGHT));
        mask = mask & (mask >> (1 * (HEIGHT)));
        score += count_bits(mask) * 100;

        // Diagonal 
        mask = bitboard & (bitboard >> (HEIGHT - 1));
        mask = mask & (mask >> (1 * (HEIGHT - 1)));
        score += count_bits(mask) * 100;

        // Diagonal /
        mask = bitboard & (bitboard >> (HEIGHT + 1));
        mask = mask & (mask >> (1 * (HEIGHT + 1)));
        score += count_bits(mask) * 100;

        // Vertical |
        mask = bitboard & (bitboard >> 1);
        mask = mask & (mask >> 1);
        score += count_bits(mask) * 100;

        // Check 2

        // Horizontal -
        mask = bitboard & (bitboard >> (HEIGHT));
        score += count_bits(mask);

        // Diagonal 
        mask = bitboard & (bitboard >> (HEIGHT - 1));
        score += count_bits(mask);

        // Diagonal /
        mask = bitboard & (bitboard >> (HEIGHT + 1));
        score += count_bits(mask);

        // Vertical |
        mask = bitboard & (bitboard >> 1);
        score += count_bits(mask);

        return score;
    }

    BitBoard get_flipped_bitboard(BitBoard bitboard)
    {
        BitBoard m1 = bitboard & k1;
        BitBoard m2 = bitboard & k2;
        BitBoard m3 = bitboard & k3;

        return (bitboard & k4)
            | ((m1 & right) << (HEIGHT * (WIDTH - 1))) | ((m1 & left) >> (HEIGHT * (WIDTH - 1)))
            | ((m2 & right) << (HEIGHT * (WIDTH - 3))) | ((m2 & left) >> (HEIGHT * (WIDTH - 3)))
            | ((m3 & right) << (HEIGHT * (WIDTH - 5))) | ((m3 & left) >> (HEIGHT * (WIDTH - 5)));
    }

} // namespace ConnectFour