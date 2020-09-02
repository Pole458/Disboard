#include "RandomPlayer.h"

#include "../Engine/IPossibleMoves.h"

#include <vector>

RandomPlayer::RandomPlayer()
{
    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    // Make a random number engine
    rng = pcg32(seed_source);
}

RandomPlayer::~RandomPlayer()
{
}

Engine::IMove* RandomPlayer::choose_move(Engine::IBoard* board)
{
    Engine::IPossibleMoves* possible_moves = board->get_possible_moves();

    // Engine::IMove* selected_move = possible_moves->move_at(rand() % possible_moves->size());

    Engine::IMove* selected_move = possible_moves->move_at(rng(possible_moves->size()));

    delete possible_moves;

    return selected_move;
}