#include "Game.h"

#include <iostream>

Game::Game()
{

}

Game::Game(IBoard* board, IPlayer* first, IPlayer* second)
{
    this->board = board;
    fist_player = first;
    second_player = second;
}

Game::~Game()
{
    
}

void Game::play(bool verbose)
{
    // if(verbose)
        // std::cout << board->to_string() << std::endl;

    while(board->should_keep_going())
    {
        IMove* move;

        if(board->turn % 2 == 1)
        {
            move = fist_player->choose_move(board);
        }
        else
        {
            move = second_player->choose_move(board);
        }

        if(verbose)
            std::cout << "Turn " << board->turn << " - " << move->to_string() << std::endl;

        board->make_move(move);
        move_history.push_back(move);

        if(verbose)
        {
            std::cout << board->to_string() << std::endl;
            // std::cin.get();
        }
    }
}