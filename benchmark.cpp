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


// Benchmarks thinking time for first move
void test_first_move(IPlayer *p2, int n)
{
    HumanPlayer p1;
    Board board;

    Engine::IMove* move = p1.choose_move(&board);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < n; i++)
    {
        board.make_move(move);

        board.make_move(p2->choose_move(&board));

        board.reset();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout
            << "Time per choice: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / n) << " [ms] per choice" << std::endl
            << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

// Benchmarks thinking time for first move for MonteCarlo players
void test_first_move_mc(MonteCarloPlayer *p2, int n)
{
    HumanPlayer p1;
    Board board;

    Engine::IMove* move = p1.choose_move(&board);

    float iterations = 0;
    float max_depth = 0;

    for(int i = 0; i < n; i++)
    {
        board.make_move(move);

        board.make_move(p2->choose_move(&board));

        iterations += p2->iterations;
        max_depth += p2->max_depth_reached;

        board.reset();
    }

    std::cout
            << "Iterations avg: " << (iterations / n) << std::endl
            << "Max Depth avg: " << (max_depth / n) << std::endl;
}

// Plays n games for benchmarking winrates
void test_games(IPlayer *p1, IPlayer *p2, int n, bool verbose)
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
            Engine::play(&board, p1, p2, verbose);

            if (board.status == Engine::IBoard::First)
                w++;
            else if (board.status == Engine::IBoard::Draw)
                d++;
        }
        else
        {
            Engine::play(&board, p2, p1, verbose);

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
              << "time: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " [s]" << std::endl
              << "time per game: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / n) << " [ms] per game" << std::endl;
}

int main()
{
    // Player 1 *********************

    MonteCarloPlayer p1(25, false);
    // PRMonteCarloPlayer p1(25, false);
    // PLMonteCarloPlayer p1(300, true);

    // MiniMaxPlayer p1(10, true);
    // PMiniMaxPlayer p1(6, true);
    // PYBWMiniMaxPlayer p1(10, false);

    // Player 2 *********************

    // MonteCarloPlayer p2(50, false);
    // PRMonteCarloPlayer p2(100, false);
    // PLMonteCarloPlayer p2(25, false);

    MiniMaxPlayer p2(6, false);
    // PMiniMaxPlayer p2(6, false);
    // PYBWMiniMaxPlayer p2(6, true);

    // Mode ********************

    test_games(&p1, &p2, 1, false);

    // test_first_move(&p1, 10);

    // test_first_move_mc(&p1, 10);

    return 0;
}