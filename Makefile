all: lexer parser main.cpp rabbix
	g++ --std=c++11 main.cpp lexer/lex.cc parser/parse.cc -o rabbix

lexer: lexer/lexer.l
	cd lexer && flexc++ lexer.l && cd ..

parser: parser/parser.y
	cd parser && bisonc++ parser.y && cd ..

clean:
	rm rabbix