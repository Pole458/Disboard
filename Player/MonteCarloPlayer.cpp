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
        
        // std::cout << " - iteration: " << total_played << std::endl;

        Node* node = root;

        // Tree traversing
        int level = 0;
        while(node->children.size() != 0)
        {

            // std::cout << "traversting tree: " << node->children.size() <<  " level: " << level <<std::endl;

            Node* selected_node;
            float highscore = -1;

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
                    // std::cout << "inverse ucb : " << ucb <<std::endl;

                    if(ucb > highscore)
                    {
                        selected_node = n;
                        highscore = ucb;
                    }
                }
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
            child->board->turn++;

            node->children.push_back(child);
        }

        // std::cout << "tree expandend: " << node->children.size() << std::endl;

        // rollout
        my_game.board = node->board->get_copy();
            
        my_game.play();

        float gain = 0;
        if(my_game.board->status == IBoard::First)
        {
            gain = 1.0f;
        }
        else if(my_game.board->status == IBoard::Draw)
        {
            gain = 0.5f;
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
        if(ucb > highscore)
        {
            selected_node = n;
            highscore = ucb;
        }

        // std::cout << n->move->to_string() << ": " << ucb << " played: " << n->played << " winrate: " << n->get_winrate() << std::endl;
    }

    // std::cout << selected_node->move->to_string() << std::endl;

    // std::cin.get();

    return selected_node->move;
}
