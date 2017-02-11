#include <iostream>
#include "parser.tab.h"

using namespace std;

int main(int argc, char* argv[]) {
	cout << " yeh " << yylval << endl;
	parse(argc, argv);
}