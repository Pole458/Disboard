
#include "Engine/Game.h"
#include "ConnectFour/ConnectFourBoard.h"
#include "Player/RandomPlayer.h"

#include <iostream>

int main()
{
    // Set up rng
    srand(time(NULL));

    ConnectFourBoard board;

    RandomPlayer p1;
    RandomPlayer p2;

    Game* game = new Game(&board, &p1, &p2);

    game->play();

    // board.reset();

    // game->play();

    return 0;
}
