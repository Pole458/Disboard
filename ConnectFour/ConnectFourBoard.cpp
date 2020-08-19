#include "ConnectFourBoard.h"

#include <iostream>

ConnectFourBoard::ConnectFourBoard()
{
    reset();
}

ConnectFourBoard::~ConnectFourBoard()
{

}

void ConnectFourBoard::reset()
{
    status = IBoard::Ongoing;

    turn = 1;

    for(int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
        for(int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
            board[rowIndex][columnIndex] = space;
}

IBoard* ConnectFourBoard::get_copy()
{
    ConnectFourBoard* copy = new ConnectFourBoard();
    copy->status = status;
    copy->turn = turn;

    for(int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
        for(int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
            copy->board[rowIndex][columnIndex] = board[rowIndex][columnIndex];

    return copy;
}

std::string ConnectFourBoard::to_string()
{
    std::string s = "A B C D E F G \n";
    
    for(int rowIndex = HEIGHT - 1; rowIndex >= 0; rowIndex--)
    {
        for(int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
        {
            s += board[rowIndex][columnIndex];
            s += ' ';
        }
        s += '\n';
    }

    return s;
}

void ConnectFourBoard::get_possible_moves(std::vector<IMove*> &possible_moves)
{
    for(int columnIndex = 0; columnIndex < WIDTH; columnIndex++)
    {
        if(board[HEIGHT - 1][columnIndex] == space)
        {
            possible_moves.push_back(new ConnectFourMove(columnIndex));
        }
    }
}

void ConnectFourBoard::make_move(IMove* move)
{
    int columnIndex = ((ConnectFourMove*)move)->column;

    for(int rowIndex = 0; rowIndex < HEIGHT; rowIndex++)
    {
        if(board[rowIndex][columnIndex] == space)
        {
            board[rowIndex][columnIndex] = (turn % 2 == 1 ? 'X' : 'O');
            break;
        }
    }

    if(check_victory(move))
    {
        status = (turn % 2 == 1 ? IBoard::First : IBoard::Second);
    }
    else if(turn == 42)
    {
        status = IBoard::Draw;
    }
    else
    {
        turn++;
    }
}

bool ConnectFourBoard::check_victory(IMove* last_move)
{
    int columnIndex = ((ConnectFourMove*)last_move)->column;

    int rowIndex;

    for(rowIndex = HEIGHT - 1; rowIndex >= 0; rowIndex--)
    {
        if(board[rowIndex][columnIndex] != space)
        {
            break;
        }
    }

    char color = board[rowIndex][columnIndex];

    // Check column
    int count = 1;
    int x, y;
    for(y =  rowIndex - 1; y >= 0; y--)
    {
        if(board[y][columnIndex] == color)
        {
            count++;
            if(count == 4)
            {
                // std::cout << "colum" << std::endl;
                return true;
            }
        }
        else break;
    }

    // Check row
    count = 0;
    for(x = 0; x < WIDTH; x++)
    {
        if(board[rowIndex][x] == color)
        {
            count++;
            if(count == 4)
            {
                // std::cout << "row" << std::endl;
                return true;
            }
        }
        else count = 0;
    }

    // Check diagonal (1,1)
    x = columnIndex - std::min(rowIndex, columnIndex);
    y = rowIndex - std::min(rowIndex, columnIndex);
    count = 0;
    while(x < WIDTH && y < HEIGHT)
    {
        if(board[y][x] == color)
        {
            count++;
            if(count == 4)
            {
                // std::cout << "diagonal (1,1)" << std::endl;
                return true;
            }
        }
        else count = 0;

        x++;
        y++;
    }

    // Check diagonal (-1,1)
    x = columnIndex + std::min(rowIndex, WIDTH - columnIndex);
    y = rowIndex - std::min(rowIndex, WIDTH - columnIndex);
    count = 0;
    while(x >= 0 && y < HEIGHT)
    {
        if(board[y][x] == color)
        {
            count++;
            if(count == 4)
            {
                // std::cout << "diagonal (-1,1)" << std::endl;
                return true;
            }
        }
        else count = 0;

        x--;
        y++;
    }

    return false;
}

bool ConnectFourBoard::should_keep_going()
{
    return status == IBoard::Ongoing;
}