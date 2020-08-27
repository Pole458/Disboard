#include "Game.h"

#include <iostream>

namespace Engine
{

    void play(IBoard *board, IPlayer *p1, IPlayer *p2, bool verbose)
    {
        if (verbose)
            std::cout << board->to_string() << std::endl;

        while (board->should_keep_going())
        {
            IMove *move;

            if (board->turn % 2 == 1)
            {
                move = p1->choose_move(board);
            }
            else
            {
                move = p2->choose_move(board);
            }

            if (verbose)
                std::cout << "Turn " << board->turn << " - " << move->to_string() << std::endl;

            board->make_move(move);

            delete move;

            if (verbose)
            {
                std::cout << board->to_string() << std::endl;
                // std::cin.get();
            }
        }

        if(verbose)
        {
            switch(board->status)
            {
                case IBoard::First:
                    std::cout << "Player 1 won!" << std::endl;
                break;
                case IBoard::Second:
                    std::cout << "Player 2 won!" << std::endl;
                break;
                case IBoard::Draw:
                    std::cout << "Draw!" << std::endl;
                break;
            }
        }
    }
} // namespace Engine
