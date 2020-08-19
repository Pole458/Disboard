#include "StudPlayer.h"


StudPlayer::StudPlayer()
{
}

StudPlayer::~StudPlayer()
{
    clear_possible_moves();
}

IMove* StudPlayer::choose_move(IBoard* board)
{

    if(first_move == NULL)
    {
        return RandomPlayer::choose_move(board);
    }
    else
    {
        IMove* t = first_move;
        first_move = NULL;
        return t;
    }   
}