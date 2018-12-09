CC = g++
CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
PROGRAM = project

all: main

run: main
	./$(PROGRAM) > ./result.data
	./$(PROGRAM) -p 3.8 > ./result2.data

main: main.cc
	$(CC) $(CFLAGS) -o $(PROGRAM) main.cc $(PFLAGS)