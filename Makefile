all: Game.o RandomPlayer.o ConnectFourBoard.o ConnectFourMove.o
	g++ -o main main.cpp Game.o RandomPlayer.o ConnectFourBoard.o ConnectFourMove.o

Game.o: Engine/Game.cpp Engine/Game.h
	g++ -c Engine/Game.cpp

RandomPlayer.o: Player/RandomPlayer.h Player/RandomPlayer.cpp
	g++ -c Player/RandomPlayer.cpp

ConnectFourBoard.o: ConnectFour/ConnectFourBoard.cpp ConnectFour/ConnectFourBoard.h
	g++ -c ConnectFour/ConnectFourBoard.cpp

ConnectFourMove.o: ConnectFour/ConnectFourMove.cpp ConnectFour/ConnectFourMove.h
	g++ -c ConnectFour/ConnectFourMove.cpp

clean:
	rm *.o