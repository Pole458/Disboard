#pragma once

#include "MiniMaxPlayer.h"

#include <omp.h>
#include <vector>

class PYBWMiniMaxPlayer : public MiniMaxPlayer
{
public:
    PYBWMiniMaxPlayer(int depth, bool verbose = false, int cache_size = 100000000);

    Engine::IMove *choose_move(Engine::IBoard *board);

private:

    std::vector<RandomPlayer> random_players;

    float pminimize(Node *node, int depth, float alpha, float beta);
    float pmaximize(Node *node, int depth, float alpha, float beta);

    float minimize(Node *node, int depth, float alpha, float beta);
    float maximize(Node *node, int depth, float alpha, float beta);

    // Scoring system
    float get_heuristic_score(Node *node, int rollouts);

    // Cache
    bool is_id_cached(Engine::board_id id);
    float get_cached_score(Engine::board_id id);
    void set_cached_score(Engine::board_id id, float score);
};