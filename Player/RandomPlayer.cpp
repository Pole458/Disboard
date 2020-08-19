#include "RandomPlayer.h"

#include <vector>

RandomPlayer::RandomPlayer()
{

}

RandomPlayer::~RandomPlayer()
{
    clear_possible_moves();
}

IMove* RandomPlayer::choose_move(IBoard* board)
{

    clear_possible_moves();

    board->get_possible_moves(possible_moves);

    int n = rand() % possible_moves.size();

    return possible_moves.at(n);
}

void RandomPlayer::clear_possible_moves()
{
    // Delete unused moves
    for(auto m : possible_moves)
    {
        delete m;
    }
    possible_moves.clear();
}