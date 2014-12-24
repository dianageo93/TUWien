# Makefile for the bucket sort sequential and parallel implementation.

CC=g++
CFLAGS=-Wall -g -c -fcilkplus
LDFLAGS=-lm -lcilkrts
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
	./$(EXE) ${ARGS}

clean:
	rm bin/*
