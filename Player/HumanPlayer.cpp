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

    for(int i = 0; i < possible_moves->size(); i++)
    {
        std::cout << i << "." << possible_moves->move_at(i)->to_string() << "  ";
    }

    std::cout << std::endl;

    std::string input;
    int x;

    getline(std::cin, input);

    while (!tryParse(input, x))
    {
        std::cout << "Please enter a number: ";
        getline(std::cin, input);
    }

    Engine::IMove* selected_move = possible_moves->move_at(x);

    delete possible_moves;

    return selected_move;
}