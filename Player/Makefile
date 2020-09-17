all: Player.a

Player.a: RandomPlayer.o HumanPlayer.o  MonteCarloPlayer.o Node.o Score.o MiniMaxPlayer.o PTMonteCarloPlayer.o PNode.o PScore.o PRMonteCarloPlayer.o PLMonteCarloPlayer.o PMiniMaxPlayer.o
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

PTMonteCarloPlayer.o: PTMonteCarloPlayer.h PTMonteCarloPlayer.cpp
	g++ -c -std=c++17 -fopenmp PTMonteCarloPlayer.cpp

PRMonteCarloPlayer.o: PRMonteCarloPlayer.h PRMonteCarloPlayer.cpp
	g++ -c -fopenmp PRMonteCarloPlayer.cpp

PLMonteCarloPlayer.o: PLMonteCarloPlayer.h PLMonteCarloPlayer.cpp
	g++ -c -fopenmp PLMonteCarloPlayer.cpp

PNode.o: PNode.h PNode.cpp
	g++ -c PNode.cpp
	
PScore.o: PScore.h PScore.cpp
	g++ -c PScore.cpp

PMiniMaxPlayer.o: PMiniMaxPlayer.h PMiniMaxPlayer.cpp
	g++ -c -fopenmp PMiniMaxPlayer.cpp

clean:
	rm -f *.o
	rm -f *.a