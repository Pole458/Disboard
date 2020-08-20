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

    IMove* selected_move = possible_moves.at(rand() % possible_moves.size())->get_copy();

    for(auto m : possible_moves)
    {
        delete m;
    }
    possible_moves.clear();

    return selected_move;
}