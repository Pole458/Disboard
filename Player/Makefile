all: Player.a

Player.a: RandomPlayer.o HumanPlayer.o
	ar -rv $@ $^

RandomPlayer.o: RandomPlayer.h RandomPlayer.cpp
	g++ -c RandomPlayer.cpp

HumanPlayer.o: HumanPlayer.h HumanPlayer.cpp
	g++ -c HumanPlayer.cpp

clean:
	rm -f *.o
	rm -f *.a