all: Game.o RandomPlayer.o ConnectFourBoard.o ConnectFourMove.o Node.o MonteCarloPlayer.o StudPlayer.o
	g++ -o main main.cpp Game.o RandomPlayer.o ConnectFourBoard.o ConnectFourMove.o Node.o MonteCarloPlayer.o StudPlayer.o

Game.o: Engine/Game.cpp Engine/Game.h
	g++ -c Engine/Game.cpp

RandomPlayer.o: Player/RandomPlayer.h Player/RandomPlayer.cpp
	g++ -c Player/RandomPlayer.cpp

ConnectFourBoard.o: ConnectFour/ConnectFourBoard.cpp ConnectFour/ConnectFourBoard.h
	g++ -c ConnectFour/ConnectFourBoard.cpp

ConnectFourMove.o: ConnectFour/ConnectFourMove.cpp ConnectFour/ConnectFourMove.h
	g++ -c ConnectFour/ConnectFourMove.cpp

Node.o: Player/Node.h Player/Node.cpp
	g++ -c Player/Node.cpp

MonteCarloPlayer.o: Player/MonteCarloPlayer.h Player/MonteCarloPlayer.cpp
	g++ -c Player/MonteCarloPlayer.cpp

StudPlayer.o: Player/StudPlayer.h Player/StudPlayer.cpp
	g++ -c Player/StudPlayer.cpp

clean:
	rm *.o