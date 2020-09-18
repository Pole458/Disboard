#include "ConnectFour/Board.h"
#include "Player/RandomPlayer.h"
#include "Player/MonteCarloPlayer.h"
#include "Player/HumanPlayer.h"
#include "Player/MiniMaxPlayer.h"
#include "Engine/Engine.h"
#include "Pcg/pcg_random.hpp"
#include "Player/PTMonteCarloPlayer.h"
#include "Player/PRMonteCarloPlayer.h"
#include "Player/PLMonteCarloPlayer.h"
#include "Player/PMiniMaxPlayer.h"

#include <iostream>
#include <chrono>
#include <random>

// Change namespace to change game
using namespace ConnectFour;

void single_game(IPlayer *p1, IPlayer *p2)
{
    Board board;
    Engine::play(&board, p1, p2, true);
}

void test_games(IPlayer *p1, IPlayer *p2, int n)
{

    int w = 0;
    int d = 0;
    float turns = 0;

    Board board;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for (int i = 0; i < n; i++)
    {
        // Balance first turn advantage
        if (i % 2)
        {
            Engine::play(&board, p1, p2);

            if (board.status == Engine::IBoard::First)
                w++;
            else if (board.status == Engine::IBoard::Draw)
                d++;
        }
        else
        {
            Engine::play(&board, p2, p1);

            if (board.status == Engine::IBoard::Second)
                w++;
            else if (board.status == Engine::IBoard::Draw)
                d++;
        }

        turns += board.turn;

        board.reset();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "played: " << n << std::endl
              << "w/d/l: " << w << "/" << d << "/" << (n - w - d) << std::endl
              << "p1 winrate: " << (w + d / 2.0f) * 100 / n << "%" << std::endl
              << "p2 winrate: " << (n - w - d / 2.0f) * 100 / n << "%" << std::endl
              << "avg.turns: " << (turns / n) << std::endl
              << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl
              << "time per game: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / n) << " [ns] per game" << std::endl;
}

int main()
{
    // HumanPlayer p1;
    RandomPlayer p1;
    // MiniMaxPlayer p1(14, true);
    // MonteCarloPlayer p1(1000000, true);
    // PTMonteCarloPlayer p1(100000, true);
    // PRMonteCarloPlayer p1(10000, true);
    // PLMonteCarloPlayer p1(10000, 4, false);

    // HumanPlayer p2;
    // RandomPlayer p2;
    // MiniMaxPlayer p2(12, true);
    // MonteCarloPlayer p2(1000, true);
    // PTMonteCarloPlayer p2(100000, true);
    // PRMonteCarloPlayer p2(10000, false);
    // PLMonteCarloPlayer p2(10000, 1, true);
    PLMonteCarloPlayer p2(1000000, 4, true);
    // PMiniMaxPlayer p2(14, true);

    // BitBoard b = 4438047605121UL;

    // std::cout << bitboard_to_string(k1) << std::endl;
    // std::cout << bitboard_to_string(k2) << std::endl;
    // std::cout << bitboard_to_string(k3) << std::endl;
    // std::cout << bitboard_to_string(k4) << std::endl;
    // std::cout << (get_flipped_bitboard(get_flipped_bitboard(b)) == b) << std::endl;


    // std::cout << bitboard_to_string(get_flipped_bitboard(get_flipped_bitboard(b))) << std::endl;

    // std::cout << b << " " << get_flipped_bitboard(get_flipped_bitboard(b)) << std::endl;

    single_game(&p1, &p2);

    // test_games(&p1, &p2, 10);

    return 0;
}