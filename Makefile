# Makefile for the radix sort sequential and parallel implementation.

CC=g++
CFLAGS=-c -Wall
SOURCES=src/vector_generator.cpp src/radix_sort.cpp src/main.cpp src/test_sort.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/seq

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
