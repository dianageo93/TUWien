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

run-1: all
	mpirun -host jupiter20 -np 1 $(EXE) ${ARGS}

run-2: all
	mpirun -host jupiter20 -np 2 $(EXE) ${ARGS}

run-16: all
	mpirun -host jupiter20 -np 16 $(EXE) ${ARGS}

run-48: all 
	mpirun -host jupiter1 -np 16 -host jupiter2 -np 16 -host jupiter3 -np 16 $(EXE) ${ARGS}

run-160: all 
	mpirun -host jupiter1 -np 16 -host jupiter2 -np 16 -host jupiter3 -np 16 -host jupiter4 -np 16 -host jupiter5 -np 16 -host jupiter6 -np 16 -host jupiter7 -np 16 -host jupiter8 -np 16 -host jupiter9 -np 16 -host jupiter10 $(EXE) ${ARGS}

run-240: all 
	mpirun -host jupiter1 -np 16 -host jupiter2 -np 16 -host jupiter3 -np 16 -host jupiter4 -np 16 -host jupiter5 -np 16 -host jupiter6 -np 16 -host jupiter7 -np 16 -host jupiter8 -np 16 -host jupiter9 -np 16 -host jupiter10 -np 16 -host jupiter11 -np 16 -host jupiter12 -np 16 -host jupiter13 -np 16 -host jupiter14 -np 16 -host jupiter15 -np 16 $(EXE) ${ARGS}

clean:
	rm bin/*
