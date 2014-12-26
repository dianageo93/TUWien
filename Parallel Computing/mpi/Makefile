# Makefile for the bucket sort sequential and parallel implementation.

CC=mpic++
CFLAGS=-Wall -g -c
LDFLAGS=-lm
EXE=bin/exec
SRC=$(wildcard src/*.cpp)
OBJ=$(subst src, bin, $(SRC:.cpp=.o))
BINFOLDER=bin

all: $(BINFOLDER) $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

bin/%.o: src/%.cpp
	$(CC) -o $@ $< $(CFLAGS)

$(BINFOLDER):
	mkdir $(BINFOLDER)

run: all 
	mpirun -host jupiter1 -np 2 $(EXE) ${ARGS}

clean:
	rm bin/*
