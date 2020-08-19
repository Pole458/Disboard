
#include "Engine/Game.h"
#include "ConnectFour/ConnectFourBoard.h"
#include "Player/RandomPlayer.h"
#include "Player/MonteCarloPlayer.h"
#include "Player/HumanPlayer.h"


#include <iostream>
#include <chrono>

void test_mc(int n)
{
    int w = 0;
    int d = 0;
    float turns = 0;

    ConnectFourBoard board;

    // RandomPlayer p1;
    HumanPlayer p1;
    // MonteCarloPlayer p1;
    MonteCarloPlayer p2;

    Game* game = new Game(&board, &p1, &p2);

    time_t start_time = time(NULL);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < n; i++)
    {
        game->play(true);

        if(board.status == IBoard::First)
            w++;
        else if(board.status == IBoard::Draw)
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

    ConnectFourBoard board;

    RandomPlayer p1;
    RandomPlayer p2;

    Game* game = new Game(&board, &p1, &p2);

    time_t start_time = time(NULL);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(int i = 0; i < n; i++)
    {
        game->play();
    
        if(board.status == IBoard::First)
            w++;
        else if(board.status == IBoard::Draw)
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

void test_random(int n, int width)
{
    int row[width];

    for(int i = 0; i < width; i++)
    {
        row[i] = 0;
    }

    for(int i = 0; i < n * width; i++)
    {
        row[rand() % width]++;
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

    // test_random(1000, 7);

    test_mc(100);

    // test_games(10000);

    return 0;
}