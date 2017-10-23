###############################################################################
# purpose: Assignment 3
# created: 10-26-2014
#  author: J. Edward Swan II
###############################################################################
LEX		=	flex
.PRECIOUS 	=	parser.cpp rules.l lexer.h 

parse.exe: lex.yy.o parser.o
	g++ -o $@ $^

lex.yy.o: lex.yy.c lexer.h
	gcc -o $@ -c lex.yy.c

parser.o: parser.cpp lexer.h
	g++ -o $@ -c parser.cpp

lex.yy.c: rules.l
	$(LEX) $^

clean: 
	$(RM) *.o lex.yy.c parse.exe

