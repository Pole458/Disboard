#pragma once

#include "../Engine/IPlayer.h"
#include "Node.h"
#include "Score.h"
#include "omp.h"

#include <unordered_map>
#include <limits>

class PMiniMaxPlayer : public IPlayer
{
public:
    PMiniMaxPlayer(int depth, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

    bool uses_scores;

private:
    int minimize(Node *node, int depth, int alpha, int beta);
    int maximize(Node *node, int depth, int alpha, int beta);

    int pminimize(Node *node, int depth, int alpha, int beta);
    int pmaximize(Node *node, int depth, int alpha, int beta);

    // Max depth at which to explore the game-tree
    int max_depth;

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, int> scores;

    // Debug / Analytics
    bool verbose;
    int nodes_evaluated;
    int pruned;
    int cached;

    omp_lock_t lock;
};