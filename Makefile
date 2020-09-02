all: Engine.a Player.a ConnectFour.a
	g++ -o main main.cpp Engine/Engine.a Player/Player.a ConnectFour/ConnectFour.a

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