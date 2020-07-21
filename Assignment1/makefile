CC=g++ -c
FLAGS= -Wall -g
Assignment1: inputbuf.cc inputbuf.h lexer.cc lexer.h inputbuf.o lexer.o
	$(CC) lexer.cc inputbuf.cc
	g++ -o a.out lexer.o inputbuf.o
clean:
	rm *.o
	rm Assignment1.out
