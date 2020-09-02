
#include "ConnectFour/Board.h"
#include "Player/RandomPlayer.h"
#include "Player/MonteCarloPlayer.h"
#include "Player/HumanPlayer.h"
#include "Engine/Engine.h"
#include "Player/MiniMaxPlayer.h"

#include "Pcg/pcg_random.hpp"

#include <iostream>
#include <chrono>
#include <random>

// Change namespace to change game
using namespace ConnectFour;

void single_game()
{
    Board board;

    HumanPlayer p2;
    // MonteCarloPlayer p2(10000, true, false);
    // MiniMaxPlayer p2(7, true);
    MiniMaxPlayer p1(12, true);
    // RandomPlayer p2;
    // HumanPlayer p2;

    Engine::play(&board, &p1, &p2, true);
}

void test_mc(int n)
{
    int w = 0;
    int d = 0;
    float turns = 0;

    Board board;

    MonteCarloPlayer p1(10000, true);
    // RandomPlayer p1;
    HumanPlayer p2;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < n; i++)
    {
        Engine::play(&board, &p1, &p2, true);

        if(board.status == Engine::IBoard::First)
            w++;
        else if(board.status == Engine::IBoard::Draw)
            d++;

        turns += board.turn;

        board.reset();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout <<
        "played: " << n << std::endl <<
        "winrate: " << (w + d / 2.0f) * 100 / n << "%" << std::endl <<
        "w/d/l: " << w << "/" << d << "/" << (n-w-d) << std::endl <<
        "avg.turns: " << (turns / n) << std::endl <<
        "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        
}

void test_games(int n)
{

    int w = 0;
    int d = 0;
    float turns = 0;

    Board board;

    // RandomPlayer p1;
    MiniMaxPlayer p1(3);
    // MonteCarloPlayer p2(1000);
    RandomPlayer p2;

    time_t start_time = time(NULL);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < n; i++)
    {
        Engine::play(&board, &p1, &p2, false);
    
        if(board.status == Engine::IBoard::First)
            w++;
        else if(board.status == Engine::IBoard::Draw)
            d++;

        turns += board.turn;

        board.reset();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout <<
        "played: " << n << std::endl <<
        "winrate: " << (w + d / 2.0f) * 100 / n << "%" << std::endl <<
        "w/d/l: " << w << "/" << d << "/" << (n-w-d) << std::endl <<
        "avg.turns: " << (turns / n) << std::endl <<
        "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl <<
        "time per game: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / n) << " [ns] per game" << std::endl;
}

void test_random(int n, int width)
{

    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    // Make a random number engine
    pcg32 rng(seed_source);

    int row[width];

    for(int i = 0; i < width; i++)
    {
        row[i] = 0;
    }

    for(int i = 0; i < n * width; i++)
    {
        row[rng(width)]++;
    }

    for(int i = 0; i < width; i++)
    {
        std::cout << row[i] << std::endl;
    }
}

int main()
{
    // Set up rng
    srand(time(NULL));
    
    single_game();

    // test_random(1000, 7);

    // test_cf();

    // test_mc(1);

    // test_games(1000);

    return 0;
}