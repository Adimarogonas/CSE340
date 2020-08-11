CC=g++ -c
FLAGS= -Wall -g
Assignment1: inputbuf.cc inputbuf.h lexer.cc lexer.h parser.cc parser.h parser.o inputbuf.o lexer.o
	$(CC) parser.cc lexer.cc inputbuf.cc -g 
	g++ -o a.out parser.o lexer.o inputbuf.o
clean:
	rm *.o
	rm a.out
