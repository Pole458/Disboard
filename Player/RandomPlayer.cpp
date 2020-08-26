#include "RandomPlayer.h"

#include "../Engine/IPossibleMoves.h"

#include <vector>

RandomPlayer::RandomPlayer()
{

}

RandomPlayer::~RandomPlayer()
{
}

Engine::IMove* RandomPlayer::choose_move(Engine::IBoard* board)
{
    Engine::IPossibleMoves* possible_moves = board->get_possible_moves();

    Engine::IMove* selected_move = possible_moves->move_at((rand() % possible_moves->size()));

    delete possible_moves;

    return selected_move;
}