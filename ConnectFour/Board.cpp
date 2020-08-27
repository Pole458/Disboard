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

        for (int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
            for (int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
                board[rowIndex][columnIndex] = space;

        // Bitboard
        both = 0UL;
        current = 0UL;
    }

    Engine::IBoard *Board::get_copy()
    {
        Board *copy = new Board();
        copy->status = status;
        copy->turn = turn;

        for (int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
            for (int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
                copy->board[rowIndex][columnIndex] = board[rowIndex][columnIndex];

        // Bitboard
        copy->both = both;
        copy->current = current;

        return copy;
    }

    void Board::copy(IBoard *board_to_copy)
    {
        status = board_to_copy->status;
        turn = board_to_copy->turn;

        Board *btc = ((Board *)board_to_copy);

        for (int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
            for (int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
                board[rowIndex][columnIndex] = btc->board[rowIndex][columnIndex];

        // Bitboard
        both = btc->both;
        current = btc->current;
    }

    std::string Board::to_string()
    {
        std::string s ="A B C D E F G \n";

        Color current_color;
        Color opposite_color;

        if(turn % 2)
        {
            current_color = red;
            opposite_color = yellow;
        }
        else
        {
            current_color = yellow;
            opposite_color = red;
        }

        BitBoard mask = 1UL;
        for (int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
        {
            for (int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
            {
                
                if(current & mask)
                    s += current_color;
                else if((both & ~current) & mask)
                    s += opposite_color;
                else 
                    s += space;

                s += ' ';

                mask = mask << 1;
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

        BitBoard move_mask = ((Move*)move)->column;

        // Find first empty spot in the selected column
        BitBoard mask = move_mask << (WIDTH * (HEIGHT - 1));
        for(; both & mask; mask = mask >> WIDTH);

        // Apply move
        current |= mask;
        both |= mask;

        if (check_victory(mask))
        {
            status = (turn % 2 == 1 ? IBoard::First : IBoard::Second);
        }
        else if (turn == 42)
        {
            status = IBoard::Draw;
        }
        else
        {
            turn++;
            // Swap current player bitboard
            current = both & (~current);
        }
    }

    bool Board::check_victory(BitBoard last_move_mask)
    {

        BitBoard mask;
        int count;

        // Check diagonal (1, -1)
        mask = last_move_mask << (WIDTH + 1);
        count = 1;
        for(; (full_board & mask) && (current & mask) && count < 4; mask <<= (WIDTH + 1), count++);
        if(count >= 4) return true;

        // Check diagonal (-1, -1)
        mask = last_move_mask << (WIDTH - 1);
        count = 1;
        for(; (full_board & mask) && (current & mask) && count < 4; mask <<= (WIDTH - 1), count++);
        if(count >= 4) return true;

        // Check diagonal (1, 1)
        mask = last_move_mask >> (WIDTH + 1);
        count = 1;
        for(; (full_board & mask) && (current & mask) && count < 4; mask >>= (WIDTH + 1), count++);
        if(count >= 4) return true;

        // Check diagonal (-1, 1)
        mask = last_move_mask >> (WIDTH - 1);
        count = 1;
        for(; (full_board & mask) && (current & mask) && count < 4; mask >>= (WIDTH - 1), count++);
        if(count >= 4) return true;

        // Check row (1, 0)
        mask = last_move_mask << 1;
        count = 1;
        for(; (full_board & mask) && (current & mask) && count < 4; mask <<= 1, count++);
        if(count >= 4) return true;

        // Check column (0, -1)
        mask = last_move_mask << WIDTH;
        count = 1;        
        for(; (full_board & mask) && (current & mask) && count < 4; mask <<= WIDTH, count++);
        if(count >= 4) return true;

        return false;

        // int columnIndex = ((Move *)last_move)->column;

        // int rowIndex;

        // for (rowIndex = HEIGHT - 1; rowIndex >= 0; rowIndex--)
        // {
        //     if (board[rowIndex][columnIndex] != space)
        //     {
        //         break;
        //     }
        // }

        // char color = board[rowIndex][columnIndex];

        // // Check column
        // int count = 1;
        // int x, y;
        // for (y = rowIndex - 1; y >= 0; y--)
        // {
        //     if (board[y][columnIndex] == color)
        //     {
        //         count++;
        //         if (count == 4)
        //         {
        //             // std::cout << "colum" << std::endl;
        //             return true;
        //         }
        //     }
        //     else
        //         break;
        // }

        // // Check row
        // count = 0;
        // for (x = 0; x < WIDTH; x++)
        // {
        //     if (board[rowIndex][x] == color)
        //     {
        //         count++;
        //         if (count == 4)
        //         {
        //             // std::cout << "row" << std::endl;
        //             return true;
        //         }
        //     }
        //     else
        //         count = 0;
        // }

        // // Check diagonal (1,1)
        // x = columnIndex - std::min(rowIndex, columnIndex);
        // y = rowIndex - std::min(rowIndex, columnIndex);
        // count = 0;
        // while (x < WIDTH && y < HEIGHT)
        // {
        //     if (board[y][x] == color)
        //     {
        //         count++;
        //         if (count == 4)
        //         {
        //             // std::cout << "diagonal (1,1)" << std::endl;
        //             return true;
        //         }
        //     }
        //     else
        //         count = 0;

        //     x++;
        //     y++;
        // }

        // // Check diagonal (-1,1)
        // x = columnIndex + std::min(rowIndex, WIDTH - columnIndex);
        // y = rowIndex - std::min(rowIndex, WIDTH - columnIndex);
        // count = 0;
        // while (x >= 0 && y < HEIGHT)
        // {
        //     if (board[y][x] == color)
        //     {
        //         count++;
        //         if (count == 4)
        //         {
        //             // std::cout << "diagonal (-1,1)" << std::endl;
        //             return true;
        //         }
        //     }
        //     else
        //         count = 0;

        //     x--;
        //     y++;
        // }

        // return false;
    }

    bool Board::should_keep_going()
    {
        return status == IBoard::Ongoing;
    }
} // namespace ConnectFour
