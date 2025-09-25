test: clean all
	./tester.sh

all: utils.o errors.o rules.o list.o lex.yy.c parser.tab.c parser.tab.h
	gcc -o parser parser.tab.c lex.yy.c utils.o errors.o rules.o list.o -lm

utils.o: utils.h
	gcc utils.c -o utils.o -c -g -Wall

errors.o: parser.tab.h errors.h 
	gcc errors.c -o errors.o -c -g -Wall

rules.o: rules.h
	gcc rules.c -o rules.o -c -g -Wall

list.o: list.h
	gcc list.c -o list.o -c -g -Wall

parser.tab.c parser.tab.h: parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

show_conflicts: parser.y
	bison -t -v -d -Wcex parser.y

clean:
	@rm -f *.o *.out *.yy.c *.tab.* *.output parser