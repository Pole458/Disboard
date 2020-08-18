#include "RandomPlayer.h"

#include <vector>

RandomPlayer::RandomPlayer()
{

}

RandomPlayer::~RandomPlayer()
{

}

IMove* RandomPlayer::choose_move(IBoard* board)
{
    std::vector<IMove*> possible_moves = board->get_possible_moves();

    int n = rand() % possible_moves.size();

    return possible_moves.at(n);
}