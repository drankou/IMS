CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
 
CC = g++

all: main

main: main.cc
	$(CC) $(CFLAGS) -o $@ main.cc $(PFLAGS)