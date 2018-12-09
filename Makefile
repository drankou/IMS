CC = g++
CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
PROGRAM = project

all: main

run:
	./$(PROGRAM) > ./result.data

main: main.cc
	$(CC) $(CFLAGS) -o $(PROGRAM) main.cc $(PFLAGS)

