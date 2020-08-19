#include "MonteCarloPlayer.h"


MonteCarloPlayer::MonteCarloPlayer()
{    
    p1 = new RandomPlayer();
    p2 = new RandomPlayer();

    my_game.fist_player = p1;
    my_game.second_player = p2;
}

MonteCarloPlayer::~MonteCarloPlayer()
{
    delete p1;
    delete p2;

    delete my_game.board;
}

IMove* MonteCarloPlayer::choose_move(IBoard* board)
{
    Node* root = new Node();
    root->parent = NULL;

    int my_turn = board->turn % 2;
    
    root->board = board->get_copy();

    int total_played = 0;
    for(; total_played < 1000; total_played++)
    {
        
        // std::cout << " -- ITERATION: " << total_played << std::endl;

        Node* node = root;

        // Tree traversing
        int level = 0;
        while(node->children.size() != 0)
        {

            // std::cout << "traversting tree: " << node->children.size() <<  " level: " << level <<std::endl;

            Node* selected_node;
            float highscore = -1;

            // std::cout << (node->board->turn % 2 == my_turn) << std::endl << node->board->to_string() << std::endl;
            // std::cin.get();

            if(node->board->turn % 2 == my_turn)
            {
                for(auto n : node->children)
                {
                    float ucb = n->get_ucb(total_played);
                    if(ucb > highscore)
                    {
                        selected_node = n;
                        highscore = ucb;
                    }
                }
            }
            else
            {
                for(auto n : node->children)
                {
                    float ucb = n->get_inverse_ucb(total_played);
                    if(ucb > highscore)
                    {
                        selected_node = n;
                        highscore = ucb;
                    }
                }

                // selected_node = node->children.at(rand() % node->children.size());
            }

            level++;
            node = selected_node;
        }

        // std::cout << "tree traversed: " << level << std::endl;

        // expand
        std::vector<IMove*> possible_moves;
        node->board->get_possible_moves(possible_moves);

        for(auto move : possible_moves)
        {
            Node* child = new Node();
            child->parent = node;
            child->move = move;

            child->board = node->board->get_copy();
            child->board->make_move(move);

            node->children.push_back(child);
        }

        // std::cout << "tree expandend: " << node->children.size() << std::endl;

        // rollout
        my_game.board = node->board->get_copy();
            
        my_game.play();

        // std::cout << "game played" << std::endl;

        float gain = 0;
        if(my_game.board->status == IBoard::Draw)
        {
            gain = 0.5f;
        }
        else if((my_game.board->status == IBoard::First && my_turn == 1) || (my_game.board->status == IBoard::Second && my_turn == 0))
        {
            gain = 1;
        }

        // std::cout << "rollout: " << gain << std::endl;

        // Backprob
        do
        {
            node->score += gain;
            node->played += 1;
            node = node->parent;
            level--;
        }
        while(node != NULL);

        // std::cout << "backpropagation: " << level << std::endl;

    }

    // Select best move
    float highscore = -1;
    Node* selected_node;

    for(auto n : root->children)
    {
        float ucb = n->get_ucb(total_played);
        float winrate = n->get_winrate();
        if(winrate > highscore)
        {
            selected_node = n;
            highscore = winrate;
        }

        // std::cout << n->move->to_string() << ": " << (winrate * 100) << "% played: " << n->played << " ucb: " << ucb << std::endl;
    }

    std::cout << selected_node->move->to_string() << std::endl;

    // std::cin.get();

    return selected_node->move;
}
