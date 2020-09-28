#pragma once

#include "MiniMaxPlayer.h"

#include "omp.h"
#include <vector>

class PMiniMaxPlayer : public MiniMaxPlayer
{
public:
    PMiniMaxPlayer(int depth, bool verbose = false, int cache_size = 100000000);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:
    std::vector<RandomPlayer> random_players;

    float minimize(Node *node, int depth, float alpha, float beta);
    float maximize(Node *node, int depth, float alpha, float beta);

    // Scoring system
    float get_heuristic_score(Node *node, int rollouts);
};