#include "HumanPlayer.h"

#include <vector>
#include <iostream>

HumanPlayer::HumanPlayer()
{

}

HumanPlayer::~HumanPlayer()
{
    clear_possible_moves();
}

bool tryParse(std::string& input, int& output) {
    try{
        output = std::stoi(input);
    } catch (std::invalid_argument) {
        return false;
    }
    return true;
}

IMove* HumanPlayer::choose_move(IBoard* board)
{
    clear_possible_moves();

    board->get_possible_moves(possible_moves);

    for(int i = 0; i < possible_moves.size(); i++)
    {
        std::cout << i << "." << possible_moves.at(i)->to_string() << "  ";
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

    return possible_moves.at(x);
}

void HumanPlayer::clear_possible_moves()
{
    // Delete unused moves
    for(auto m : possible_moves)
    {
        delete m;
    }
    possible_moves.clear();
}