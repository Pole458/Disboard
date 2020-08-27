#pragma once

#include <cstdint>
#include <string>

#define WIDTH 7
#define HEIGHT 7

// forward declarations
namespace ConnectFour
{
    using BitBoard = uint64_t;

    // const BitBoard upper_row_mask = (1UL << HEIGHT) - 1UL; // 0b1111111; // (2 ^ 8) - 1 = 127
    const BitBoard upper_row_mask = 283691315109952UL;
    // const BitBoard full_board = (1UL << (WIDTH * HEIGHT)) - 1UL;

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
}