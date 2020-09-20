#pragma once

#include "../Engine/IPlayer.h"
#include "RandomPlayer.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>
#include <limits>
#include <omp.h>

class PYBWMiniMaxPlayer : public IPlayer
{
public:
    PYBWMiniMaxPlayer(int depth, bool verbose = false, int cache_size = 100000000);

    Engine::IMove *choose_move(Engine::IBoard *board);

private:
    RandomPlayer random_player;

    float pminimize(Node *node, int depth, float alpha, float beta);
    float pmaximize(Node *node, int depth, float alpha, float beta);

    float minimize(Node *node, int depth, float alpha, float beta);
    float maximize(Node *node, int depth, float alpha, float beta);

    // Scoring system
    float get_score(Node* node);
    float get_heuristic_score(Node *node, int rollouts);

    // Cache
    int cache_size;
    bool is_id_cached(Engine::board_id id);
    float get_cached_score(Engine::board_id id);
    void set_cached_score(Engine::board_id id, float score);
   
    std::unordered_map<Engine::board_id, Engine::board_id> cached_id;
    std::unordered_map<Engine::board_id, float> cached_scores;

    // Max depth at which to explore the game-tree
    int max_depth;

    int my_turn;

    // Debug / Analytics
    bool verbose;
    int leafs_reached;
    int nodes_evaluated;
    int nodes_pruned;
    int cache_hits;
};