CC = gcc
CFLAGS += -c -Wall -ansi -pedantic -std=c99 -I ./cii/include -g
LDFLAGS = -L./cii
LDLIBS = -lcii

lexer_test: lexer.o lexer_test.o

interpreter: lexer.o repl.o interpreter.o

all: interpreter lexer_test

clean:
	rm -rf *.o
	-rm lexer_test
	-rm interpreter

.PHONY: all

