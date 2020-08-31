CC = gcc
CFLAGS += -c -Wall -ansi -pedantic -std=c99 -I ./cii/include -g
LDFLAGS = -L./cii
LDLIBS = -lcii

all: lexer_test parser_test interpreter

lexer_test: token.o lexer.o lexer_test.o

interpreter: token.o lexer.o ast.o parser.o repl.o interpreter.o

parser_test: token.o lexer.o ast.o parser.o parser_test.o

clean:
	rm -rf *.o
	-rm lexer_test
	-rm parser_test
	-rm interpreter

.PHONY: all

