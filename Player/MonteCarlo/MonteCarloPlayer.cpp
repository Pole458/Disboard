#include "MonteCarloPlayer.h"

#include "../../Engine/IPossibleMoves.h"
#include "BoardPosition.h"

#include <unordered_map>


MonteCarloPlayer::MonteCarloPlayer(int rollouts, bool verbose, bool step_by_step)
{
    player = new RandomPlayer();
    _rollouts = rollouts;
    _verbose = verbose;
    _step_by_step = step_by_step;
}

MonteCarloPlayer::~MonteCarloPlayer()
{
    delete player;
}

Engine::IMove *MonteCarloPlayer::choose_move(Engine::IBoard *board)
{

    Node root = Node(board->get_copy());

    std::unordered_map<Engine::board_id, BoardPosition> positions;

    positions.emplace(root.id, root.id);

    int my_turn = board->turn % 2;
    int max_depth = 0;

    if(_step_by_step)
    {
        std::cout << "Root node: " << root.id << std::endl << std::endl;
        std::cout.flush();
    }

    while (positions[root.id].played < _rollouts)
    {

        Node *node = &root;

        // Tree traversing
        int depth = 0;

        if(_step_by_step)
            std::cout << " 1) Traversing tree:\n    " << node->id;

        while (!node->is_leaf)
        {

            Node *selected_node;
            float highscore = -1;
            float score;

            if (node->board->turn % 2 == my_turn)
            {
                for(int i = 0; i < node->possible_moves->size(); i++)
                {
                    Node* n = node->children[i];
                    score = positions[n->id].get_ucb(positions[root.id].played);
                    if (score > highscore)
                    {
                        selected_node = n;
                        highscore = score;
                    }
                }
            }
            else
            {
                for(int i = 0; i < node->possible_moves->size(); i++)
                {
                    Node* n = node->children[i];
                    score = positions[n->id].get_inverse_ucb(positions[root.id].played);
                    if (score > highscore)
                    {
                        selected_node = n;
                        highscore = score;
                    }
                }
            }

            if(_step_by_step)
            {
                std::cout << " -> " << node->id;
                std::cout.flush();
            }

            depth++;
            node = selected_node;
        }

        if(_step_by_step)
            std::cout << std::endl;

        if (max_depth < depth)
        {
            max_depth = depth;
        }

        if(_step_by_step)
        {
            std::cout << " 2) Expanding node " << node->id << ":\n    children: ";
            std::cout.flush();
        }

        // Expand
        if(node->possible_moves->size() > 0)
        {
            node->children = new Node*[node->possible_moves->size()];
            for (int i = 0; i < node->possible_moves->size(); i++)
            {
                Node *child = new Node(node->board->get_copy(), node->possible_moves->move_at(i), node);
                node->children[i] = child;

                if(_step_by_step)
                    std::cout << child->move->to_string() << ": " << child->id << " ";
            }
            node->is_leaf = false;
        }

        if(_step_by_step)
            std::cout << std::endl;

        // Rollout

        Engine::IBoard *test_board = node->board->get_copy();
        Engine::play(test_board, player, player);

        float gain = 0;
        if (test_board->status == Engine::IBoard::Draw)
        {
            gain = 0.5f;
        }
        else if ((test_board->status == Engine::IBoard::First && my_turn == 1) || (test_board->status == Engine::IBoard::Second && my_turn == 0))
        {
            gain = 1;
        }

        delete test_board;

        if(_step_by_step)
            std::cout << " 3) Rollout: " << gain << std::endl;

        if(_step_by_step)
            std::cout << " 4) Backpropagation:" << std::endl;

        // Backprob
        do
        {  
            positions[node->id].score += gain;
            positions[node->id].played += 1;

            if(_step_by_step)
                std::cout << "    " << node->id << ": " <<
                positions[node->id].score << "/" << positions[node->id].played << std::endl;

            node = node->parent;
            depth--;
        } while (node != NULL);


        if(_step_by_step)
            std::cin.get();

    }

    // Select best move
    float highscore = -1;
    Node *selected_node;

    if(_verbose)
        std::cout << "max_depth: " << max_depth << std::endl;

    for(int i = 0; i < root.possible_moves->size(); i++)
    {
        Node* n = root.children[i];
        float ucb = positions[n->id].get_ucb(positions[root.id].played);
        float winrate = positions[n->id].get_winrate();
        if (winrate > highscore)
        {
            selected_node = n;
            highscore = winrate;
        }

        if(_verbose)
            std::cout << n->move->to_string() << ": " << (winrate * 100) << "% played: " << positions[n->id].played << " ucb: " << ucb << std::endl;
    }  

    Engine::IMove *selected_move = selected_node->move->get_copy();

    return selected_move;
}