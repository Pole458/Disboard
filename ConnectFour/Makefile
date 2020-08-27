all: ConnectFour.a

ConnectFour.a: ConnectFour.o Board.o Move.o PossibleMoves.o
	ar -rv $@ $^

ConnectFour.o: ConnectFour.h ConnectFour.cpp
	g++ -c ConnectFour.cpp

Board.o: Board.h Board.cpp
	g++ -c Board.cpp

ConnectFourMove.o: Move.h Move.cpp
	g++ -c Move.cpp

PossibleMoves.o: PossibleMoves.h PossibleMoves.cpp
	g++ -c PossibleMoves.cpp

clean:
	rm -f *.o
	rm -f *.a
