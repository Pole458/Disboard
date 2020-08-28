all: Engine.a

Engine.a: Engine.o
	ar -rv $@ $^

Engine.o: Engine.cpp Engine.h
	g++ -c Engine.cpp

clean:
	rm -f *.o
	rm -f *.a
