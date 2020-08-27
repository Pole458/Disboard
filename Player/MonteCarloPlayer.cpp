#include "MonteCarloPlayer.h"

#include "../Engine/IPossibleMoves.h"

MonteCarloPlayer::MonteCarloPlayer(int rollouts, bool verbose)
{
    player = new RandomPlayer();
    _rollouts = rollouts;
    _verbose = verbose;
}

MonteCarloPlayer::~MonteCarloPlayer()
{
    delete player;
}

Engine::IMove *MonteCarloPlayer::choose_move(Engine::IBoard *board)
{

    Node root = Node(board->get_copy());

    int my_turn = board->turn % 2;

    int max_depth = 0;

    while (root.played < _rollouts)
    {

        // std::cout << " -- ITERATION: " << total_played << std::endl;

        Node *node = &root;

        // Tree traversing
        int depth = 0;
        while (node->children.size() != 0)
        {

            // std::cout << "traversting tree: " << node->children.size() <<  " level: " << level <<std::endl;

            Node *selected_node;
            float highscore = -1;
            float score;

            // std::cout << (node->board->turn % 2 == my_turn) << std::endl << node->board->to_string() << std::endl;
            // std::cin.get();

            if (node->board->turn % 2 == my_turn)
            {
                for (auto n : node->children)
                {
                    score = n->get_ucb(root.played);
                    if (score > highscore)
                    {
                        selected_node = n;
                        highscore = score;
                    }
                }
            }
            else
            {
                for (auto n : node->children)
                {
                    score = n->get_inverse_ucb(root.played);
                    // score = n->get_inverse_winrate();
                    // score = n->get_ucb(total_played);
                    if (score > highscore)
                    {
                        selected_node = n;
                        highscore = score;
                    }
                }
            }

            depth++;
            node = selected_node;
        }

        if (max_depth < depth)
        {
            max_depth = depth;
        }

        // std::cout << "tree traversed: " << level << std::endl;

        // expand
        Engine::IPossibleMoves *possible_moves = node->board->get_possible_moves();
        for (int i = 0; i < possible_moves->size(); i++)
        {
            Node *child = new Node(node->board->get_copy(), possible_moves->move_at(i), node);
            node->children.push_back(child);

            child->board->make_move(child->move);
        }
        delete possible_moves;

        // std::cout << "tree expandend: " << node->children.size() << std::endl;

        // rollout

        Engine::IBoard *test_board = node->board->get_copy();
        Engine::play(test_board, player, player);

        // std::cout << "game played" << std::endl;

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

        // std::cout << "rollout: " << gain << std::endl;

        // Backprob
        do
        {
            node->score += gain;
            node->played += 1;
            node = node->parent;
            depth--;
        } while (node != NULL);

        // std::cout << "backpropagation: " << level << std::endl;
    }

    // Select best move
    float highscore = -1;
    Node *selected_node;

    if(_verbose)
        std::cout << "max_depth: " << max_depth << std::endl;

    for (Node *n : root.children)
    {
        float ucb = n->get_ucb(root.played);
        float winrate = n->get_winrate();
        if (winrate > highscore)
        {
            selected_node = n;
            highscore = winrate;
        }

        if(_verbose)
            std::cout << n->move->to_string() << ": " << (winrate * 100) << "% played: " << n->played << " ucb: " << ucb << std::endl;
    }

    // std::cout << selected_node->move->to_string() << std::endl;
    // std::cin.get();

    Engine::IMove *selected_move = selected_node->move->get_copy();

    return selected_move;
}
