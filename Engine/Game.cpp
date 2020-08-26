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
    }
} // namespace Engine
