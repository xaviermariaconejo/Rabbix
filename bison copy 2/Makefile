all:
	flex -o scanner.cpp scanner.l
	bison -o parser.cpp parser.y
	g++ --std=c++11 main.cpp scanner.cpp parser.cpp atn.cpp astn.cpp atnn.cpp data.cpp -o a.out

clean:
	rm -rf scanner.cpp
	rm -rf parser.cpp parser.hpp location.hh position.hh stack.hh
	rm -rf a.out