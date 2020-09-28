all: Player.a

Player.a: RandomPlayer.o HumanPlayer.o  MonteCarloPlayer.o Node.o Score.o MiniMaxPlayer.o PRMonteCarloPlayer.o PLMonteCarloPlayer.o PMiniMaxPlayer.o PYBWMiniMaxPlayer.o
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

PRMonteCarloPlayer.o: PRMonteCarloPlayer.h PRMonteCarloPlayer.cpp
	g++ -c -fopenmp PRMonteCarloPlayer.cpp

PLMonteCarloPlayer.o: PLMonteCarloPlayer.h PLMonteCarloPlayer.cpp
	g++ -c -fopenmp PLMonteCarloPlayer.cpp

PMiniMaxPlayer.o: PMiniMaxPlayer.h PMiniMaxPlayer.cpp
	g++ -c -fopenmp PMiniMaxPlayer.cpp

PYBWMiniMaxPlayer.o : PYBWMiniMaxPlayer.h PYBWMiniMaxPlayer.cpp
	g++ -c -fopenmp PYBWMiniMaxPlayer.cpp

clean:
	rm -f *.o
	rm -f *.a