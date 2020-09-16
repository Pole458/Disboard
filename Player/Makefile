all: Player.a

Player.a: RandomPlayer.o HumanPlayer.o  MonteCarloPlayer.o Node.o Score.o MiniMaxPlayer.o PMonteCarloPlayer.o PNode.o PScore.o
	ar -rv $@ $^

RandomPlayer.o: RandomPlayer.h RandomPlayer.cpp
	g++ -c RandomPlayer.cpp

HumanPlayer.o: HumanPlayer.h HumanPlayer.cpp
	g++ -c HumanPlayer.cpp

MonteCarloPlayer.o: MonteCarloPlayer.h MonteCarloPlayer.cpp
	g++ -c MonteCarloPlayer.cpp

Node.o: Node.h Node.cpp
	g++ -c Node.cpp

Score.o: Score.h Score.cpp
	g++ -c Score.cpp

MiniMaxPlayer.o: MiniMaxPlayer.h MiniMaxPlayer.cpp
	g++ -c MiniMaxPlayer.cpp

PMonteCarloPlayer.o: PMonteCarloPlayer.h PMonteCarloPlayer.cpp
	g++ -c -fopenmp PMonteCarloPlayer.cpp

PNode.o: PNode.h PNode.cpp
	g++ -c PNode.cpp
	
PScore.o: PScore.h PScore.cpp
	g++ -c PScore.cpp

clean:
	rm -f *.o
	rm -f *.a