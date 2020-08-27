#include "HumanPlayer.h"

#include <vector>
#include <iostream>

#include "../Engine/IPossibleMoves.h"

HumanPlayer::HumanPlayer()
{

}

HumanPlayer::~HumanPlayer()
{
}

bool tryParse(std::string& input, int& output) {
    try{
        output = std::stoi(input);
    } catch (std::invalid_argument) {
        return false;
    }
    return true;
}

Engine::IMove* HumanPlayer::choose_move(Engine::IBoard* board)
{


    Engine::IPossibleMoves* possible_moves = board->get_possible_moves();

    std::cout << possible_moves->to_string() << std::endl;

    // for(int i = 0; i < possible_moves->size(); i++)
    // {
    //     std::cout << i << "." << possible_moves->move_at(i)->to_string() << "  ";
    // }

    // std::cout << std::endl;

    std::string input;
    int x = -1;

    getline(std::cin, input);

    while(!tryParse(input, x) || x < 0 || x >= possible_moves->size())
    {
        std::cout << "Please enter a number between 0 and " << (possible_moves->size() - 1) << ": ";
        getline(std::cin, input);
    }

    Engine::IMove* selected_move = possible_moves->move_at(x);

    delete possible_moves;

    return selected_move;
}