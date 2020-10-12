#include "ConnectFour/Board.h"
#include "Player/RandomPlayer.h"
#include "Player/MonteCarloPlayer.h"
#include "Player/HumanPlayer.h"
#include "Player/MiniMaxPlayer.h"
#include "Engine/Engine.h"
#include "Engine/IMove.h"
#include "Pcg/pcg_random.hpp"
#include "Player/PRMonteCarloPlayer.h"
#include "Player/PLMonteCarloPlayer.h"
#include "Player/PMiniMaxPlayer.h"
#include "Player/PYBWMiniMaxPlayer.h"

#include <iostream>
#include <chrono>
#include <random>


// Change namespace to change game
using namespace ConnectFour;

// Plays a single game between two players
void single_game(IPlayer *p1, IPlayer *p2)
{
    Board board;
    Engine::play(&board, p1, p2, true);
}


int main()
{
    // Player 1 *********************

    HumanPlayer p1;
    // RandomPlayer p1;

    // MonteCarloPlayer p1(50, true); 
    // PRMonteCarloPlayer p1(50 / omp_get_max_threads(), true);
    // PLMonteCarloPlayer p1(50, true);

    // MonteCarloPlayer's strength can be set with thinking time.
    // 

    // MiniMaxPlayer p1(6, true);
    // PMiniMaxPlayer p1(6, true);
    // PYBWMiniMaxPlayer p1(6, true);

    // Player 2 *********************

    // HumanPlayer p2;
    // RandomPlayer p2;

    MonteCarloPlayer p2(50, true);
    // PRMonteCarloPlayer p2(50 / omp_get_max_threads(), true);
    // PLMonteCarloPlayer p2(50, true);

    // MiniMaxPlayer p2(6, true);
    // PMiniMaxPlayer p2(6, true);
    // PYBWMiniMaxPlayer p2(6, true);

    // Mode ********************

    single_game(&p1, &p2);

    return 0;
}