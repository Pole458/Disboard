all: Play Benchmark

Play: Engine.a Player.a ConnectFour.a play.cpp
	g++ -o play -fopenmp play.cpp Engine/Engine.a Player/Player.a ConnectFour/ConnectFour.a

Benchmark: Engine.a Player.a ConnectFour.a benchmark.cpp
	g++ -o benchmark -fopenmp benchmark.cpp Engine/Engine.a Player/Player.a ConnectFour/ConnectFour.a

Engine.a:
	cd Engine; make

Player.a:
	cd Player; make

ConnectFour.a:
	cd ConnectFour; make

clean:
	rm -f main
	rm -f *.o
	cd Engine; make clean
	cd Player; make clean
	cd ConnectFour; make clean