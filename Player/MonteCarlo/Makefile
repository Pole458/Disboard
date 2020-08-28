all: MonteCarlo.a

MonteCarlo.a: MonteCarloPlayer.o Node.o BoardPosition.o
	ar -rv $@ $^

MonteCarloPlayer.o: MonteCarloPlayer.h MonteCarloPlayer.cpp
	g++ -c MonteCarloPlayer.cpp

Node.o: Node.h Node.cpp
	g++ -c Node.cpp

BoardPosition.o: BoardPosition.h BoardPosition.cpp
	g++ -c BoardPosition.cpp

clean:
	rm -f *.o
	rm -f *.a