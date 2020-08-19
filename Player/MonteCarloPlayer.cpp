#include "MonteCarloPlayer.h"


MonteCarloPlayer::MonteCarloPlayer()
{    
    p1 = new StudPlayer();
    p2 = new StudPlayer();

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
    
    root->board = board->get_copy();

    std::vector<IMove*> possible_moves;

    root->board->get_possible_moves(possible_moves);

    // Creates a child for each possible move
    for(auto move : possible_moves)
    {
        Node* node = new Node();
        node->parent = root;
        node->move = move;

        node->board = root->board->get_copy();
        node->board->make_move(move);
        
        node->board->turn++;

        root->children.push_back(node);
    }

    IMove* selected_move;
    float highscore = -1;

    for(auto node : root->children)
    {
        for(int i = 0; i < 100; i++)
        {
            my_game.board = node->board->get_copy();

            if(my_game.board->turn % 2 == 1)
            {
                p1->first_move = node->move;
                p2->first_move = NULL;
            }
            else
            {
                p1->first_move = NULL;
                p2->first_move = node->move;
            }
            
            my_game.play();

            if(my_game.board->status == IBoard::First)
            {
                node->score += 1.0f;
            }
            else if(my_game.board->status == IBoard::Draw)
            {
                node->score += 0.5f;
            }   
        }

        // std::cout << node->move->to_string() << ": " << node->score << std::endl;
       
        if(node->score > highscore)
        {
            selected_move = node->move;
            highscore = node->score;
        }
    }

    // std::cin.get();

    return selected_move;
}
