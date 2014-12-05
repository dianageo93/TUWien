# Makefile for the bucket sort sequential and parallel implementation.

CC=g++
CFLAGS=-Wall -g -c
LDFLAGS=-lpthread -lm
EXE=bin/exec
SRC=$(wildcard src/*.cpp)
OBJ=$(subst src, bin, $(SRC:.cpp=.o))

all: $(SRC) $(EXE) $(BINFOLDER)

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
