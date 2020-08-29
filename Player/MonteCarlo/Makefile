all: MonteCarlo.a

MonteCarlo.a: MonteCarloPlayer.o Node.o Score.o
	ar -rv $@ $^

MonteCarloPlayer.o: MonteCarloPlayer.h MonteCarloPlayer.cpp
	g++ -c MonteCarloPlayer.cpp

Node.o: Node.h Node.cpp
	g++ -c Node.cpp

Score.o: Score.h Score.cpp
	g++ -c Score.cpp

clean:
	rm -f *.o
	rm -f *.a