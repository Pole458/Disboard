#pragma once

#include <cstdint>
#include <string>

#define WIDTH 7
#define HEIGHT 6

// forward declarations
namespace ConnectFour
{
    using BitBoard = uint64_t;

    const BitBoard upper_row_mask = (1UL << WIDTH) - 1UL; // 0b1111111; // (2 ^ 8) - 1 = 127
    const BitBoard full_board = (1UL << (WIDTH * HEIGHT)) - 1UL;

    using Color = char;

    class Board;
    class PossibleMoves;
    class Move;

    const Color red = 'X';
    const Color yellow = '0';
    const Color space = '*';

    std::string bitboard_to_string(BitBoard bitboard);

    int count_bits(BitBoard bitboard);
    
}