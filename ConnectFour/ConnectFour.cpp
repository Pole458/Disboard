#include "ConnectFour.h"

#include <iostream>

namespace ConnectFour
{
    std::string bitboard_to_string(BitBoard bitboard)
    {
                
        std::string s ="A B C D E F G \n";

        // BitBoard mask = 1 << (columnIndex + (rowIndex * WIDTH));
        BitBoard mask = 1UL;
        for (int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
        {
            for (int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
            {
                
                if(bitboard & mask)
                    s += '1';
                else 
                    s += '0';

                s += ' ';

                mask = mask << 1;
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
}