all: Engine.a

Engine.a: Game.o
	ar -rv $@ $^

Game.o: Game.cpp Game.h
	g++ -c Game.cpp

clean:
	rm -f *.o
	rm -f *.a
