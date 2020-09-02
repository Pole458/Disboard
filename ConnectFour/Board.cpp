#include "Board.h"

#include <iostream>

namespace ConnectFour
{

    Board::Board()
    {
        reset();
    }

    Board::~Board()
    {
    }

    void Board::reset()
    {
        status = IBoard::Ongoing;
        turn = 1;

        both = 0UL;
        current = 0UL;
    }

    Engine::IBoard *Board::get_copy()
    {
        Board *copy = new Board();

        copy->status = status;
        copy->turn = turn;

        copy->both = both;
        copy->current = current;

        return copy;
    }

    void Board::copy(IBoard *board_to_copy)
    {
        status = board_to_copy->status;
        turn = board_to_copy->turn;

        Board *btc = ((Board *)board_to_copy);

        both = btc->both;
        current = btc->current;
    }

    std::string Board::to_string()
    {
        std::string s = "A B C D E F G \n";

        Color current_color;
        Color opposite_color;

        if (turn % 2 == 0)
        {
            current_color = red;
            opposite_color = yellow;
        }
        else
        {
            current_color = yellow;
            opposite_color = red;
        }

        for (int row_index = HEIGHT - 2; row_index >= 0; row_index--)
        {
            for (int column_index = 0; column_index < WIDTH; column_index++)
            {
                BitBoard mask = mask_at(row_index, column_index);
                if (current & mask)
                    s += current_color;
                else if ((both & ~current) & mask)
                    s += opposite_color;
                else
                    s += space;

                s += ' ';
            }
            s += '\n';
        }

        return s;
    }

    Engine::IPossibleMoves *Board::get_possible_moves()
    {
        return new PossibleMoves(this);
    }

    void Board::make_move(Engine::IMove *move)
    {
        int column_index = ((Move *)move)->column;
        
        both |= both + bottom_mask(column_index);

        if (check_victory(both & (~current)))
        {
            status = (turn % 2 ? IBoard::First : IBoard::Second);
        }
        else if (turn == 42)
        {
            status = IBoard::Draw;
        }
        else
        {
            turn++;
            current ^= both;
        }
    }

    bool Board::should_keep_going()
    {
        return status == IBoard::Ongoing;
    }

    Engine::board_id Board::get_id()
    {
        // return both + current + bottom_row_mask;
        
        // BitBoard flipped_both = get_flipped_bitboard(both);
        // BitBoard flipper_current = get_flipped_bitboard(current);

        BitBoard key = both + current + bottom_row_mask;
        BitBoard flipped_key = get_flipped_bitboard(key);
        return key < flipped_key ? key : flipped_key;
    }

    int Board::get_score()
    {
        return ConnectFour::get_score(both & (~current)) - ConnectFour::get_score(both & current);
    }
} // namespace ConnectFour
