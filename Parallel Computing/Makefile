# Makefile for the radix sort sequential and parallel implementation.

CC=g++
CFLAGS=-Wall -g
SOURCES=src/*.cpp
EXESEQ=bin/seq
BINFOLDER=bin/

seq: $(SOURCES) $(BINFOLDER)
	$(CC) $(SOURCES) -o $(EXESEQ) $(CFLAGS)

$(BINFOLDER):
	mkdir $(BINFOLDER)

run-seq: seq
	./$(EXESEQ) ${ARGS}
