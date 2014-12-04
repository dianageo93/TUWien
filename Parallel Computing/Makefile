# Makefile for the bucket sort sequential and parallel implementation.

CC=g++
CFLAGS=-Wall -g -lpthread -lm
SOURCES=src/*.cpp
EXESEQ=bin/seq
BINFOLDER=bin/

all: $(SOURCES) $(BINFOLDER)
	$(CC) $(SOURCES) -o $(EXESEQ) $(CFLAGS)

$(BINFOLDER):
	mkdir $(BINFOLDER)

run: all 
	./$(EXESEQ) ${ARGS}
