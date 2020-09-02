#pragma once

#include <cstdint>
#include <string>

#define WIDTH 7
#define HEIGHT 7

// forward declarations
namespace ConnectFour
{
    using BitBoard = uint64_t;


    const BitBoard upper_row_mask =     0b0100000010000001000000100000010000001000000100000UL;
    const BitBoard bottom_row_mask =    0b0000001000000100000010000001000000100000010000001UL;

    const BitBoard k1 = 558551906910335UL;  // 100 0 001
    const BitBoard k2 =                 0b0000000011111100000000000000000000001111110000000UL;  // 010 0 010
    const BitBoard k3 = 34093383680UL;      // 001 0 100
    const BitBoard k4 = 266338304Ul;        // 000 1 000
    const BitBoard left =               0b0111111011111101111110000000000000000000000000000UL;
    const BitBoard right =              0b0000000000000000000000000000011111101111110111111UL;

    using Color = char;

    class Board;
    class PossibleMoves;
    class Move;

    const Color red = 'X';
    const Color yellow = '0';
    const Color space = '*';

    std::string bitboard_to_string(BitBoard bitboard);

    BitBoard top_mask(int column);
    BitBoard bottom_mask(int column);

    int count_bits(BitBoard bitboard);

    BitBoard mask_at(int row, int column);

    bool check_victory(BitBoard bitboard);
    int get_score(BitBoard bitboard);

    BitBoard get_flipped_bitboard(BitBoard bitboard);
}