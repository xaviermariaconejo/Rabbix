%{
	#include <cstdio>
	#include <iostream>
	using namespace std;

	// stuff from flex that bison needs to know about
	extern "C" int yylex();
	extern "C" int yyparse();
	extern "C" FILE *yyin;
	extern int line_num;
	 
	void yyerror(const char *s);
%}

%union {
	int ival;
	float fval;
	char *sval;
}

//%token 	COMMA SEMICOLON COLON ANDPERSAND OPARENTHESIS CPARENTHESIS OBRACKET CBRACKET OBRACER CBRACER
//%token 	IF ELSE WHILE DO FOR FUNC RETURN GLOBAL
%token 	<ival> BOOL INT
%token  <fval> DOUBLE
%token  <sval> STRING ID
/*
%right 	ASSIGMENT
%left	OR
%left	AND
%left	EQUAL NOT_EQUAL LT LE GT GE
%left	PLUS MINUS
%left	MULT DIV MOD
%left	NEG POS NOT
%right	POW*/


%%


expr:
	BOOL
	{
		cout << "BOOL: " << $1 << endl;
	}
|
	INT
	{
		cout << "INT: " << $1 << endl;

//		$$.i = stoi(d_scanner.matched());
//		std::cout << "\t INT: " << $$.i << '\n';
	}
|
	DOUBLE
	{
		cout << "DOUBLE: " << $1 << endl;

//		$$.d = stod(d_scanner.matched());
//		std::cout << "\t DOUBLE: " << $$.d << '\n';
	}
||
	STRING
	{
		cout << "STRING: " << $1 << endl;

//		$$.d = stod(d_scanner.matched());
//		std::cout << "\t DOUBLE: " << $$.d << '\n';
	}
|
	ID
	{
		cout << "ID: " << $1 << endl;

/*		$$.p = static_cast<void*>(new std::string(d_scanner.matched()));
		void* vp = static_cast<void*>(new std::string(d_scanner.matched()));
		std::string* sp = static_cast<std::string*>(vp);
		std::string s = *sp;
		std::cout << "\t ID: " << s << '\n';
*/	}
;

%%


int main(int, char**) {
	/*
	// open a file handle to a particular file:
	FILE *myfile = fopen("in.snazzle", "r");
	// make sure it's valid:
	if (!myfile) {
		cout << "I can't open a.snazzle.file!" << endl;
		return -1;
	}
	// set lex to read from it instead of defaulting to STDIN:
	yyin = myfile;
	*/
	
	// parse through the input until there is no more:
	
	do {
		yyparse();
	} while (!feof(yyin));
	
}

void yyerror(const char *s) {
	cout << "EEK, parse error on line " << line_num << "!  Message: " << s << endl;
	// might as well halt now:
	exit(-1);
}