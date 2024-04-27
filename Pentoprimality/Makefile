CFLAGS ?= -std=c99 -pedantic -O2 -W -Wall -Wextra -Wno-unused-parameter

all: pentoprimality ws

pentoprimality: dancing.c dancing.h pentoprimality.c
	$(CC) $(CFLAGS) -o $@ dancing.c pentoprimality.c

ws: ws.cpp dancing.c dancing.h
	$(CC) -DNDEBUG -O2 -march=native -c dancing.c
	$(CXX) -DNDEBUG -O2 -std=c++2b -march=native -c ws.cpp
	$(CXX) -DNDEBUG -O2 -std=c++2b -march=native ws.o dancing.o -o ws

clean:
	rm -f *.o pentoprimality ws

.PHONY: all clean
