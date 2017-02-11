parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

rabbix: lex.yy.c parser.tab.c
	g++ --std=c++11 parser.tab.c lex.yy.c -o rabbix

clean:
	rm rabbix parser.tab.h parser.tab.c lex.yy.c