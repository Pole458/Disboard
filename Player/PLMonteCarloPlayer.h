#pragma once


#include "RandomPlayer.h"
#include "../Engine/Engine.h"
#include "Node.h"
#include "Score.h"

#include <unordered_map>
#include <unordered_set>
#include <omp.h>

class PLMonteCarloPlayer : public IPlayer
{

public:
    PLMonteCarloPlayer(int rollouts, int rollout_size, bool verbose = false);
    Engine::IMove *choose_move(Engine::IBoard *board);

private:

    // Hash map used to store scoring for each possible board configuration.
    std::unordered_map<Engine::board_id, Score> scores;

    std::unordered_map<Engine::board_id, std::unordered_set<Node*>> nodes;

    // Queues of ids to update
    std::unordered_map<Engine::board_id, float> gains;
    std::unordered_map<Engine::board_id, int> playeds;

    // Internal random player used to for random rollout
    RandomPlayer player;

    // Number to rollout needed to choose a move
    int rollouts;

    // Number to rollout done for each iteration of the algorithm
    int rollout_size;

    bool verbose;

    void backprop();
};