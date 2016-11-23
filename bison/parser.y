%{
	#include <cstdio>
	#include <iostream>
	#include "utils/node.h"
	#include "utils/tree.h"
	using namespace std;

	// stuff from flex that bison needs to know about
	extern "C" int yylex();
	extern "C" int yyparse();
	extern "C" FILE *yyin;
	extern int line_num;
	 
	void yyerror(const char *s);
%}

%union {
	int 	ival;
	float 	fval;
	char* 	sval;

	void*	n;
}

%token 	COMMA SEMICOLON COLON ANDPERSAND OPARENTHESIS CPARENTHESIS OBRACKET CBRACKET OBRACER CBRACER NOT_DEFINED
%token 	IF ELSE WHILE DO FOR FUNC RETURN GLOBAL
%token 	<ival> BOOL INT
%token  <fval> DOUBLE
%token  <sval> STRING ID

%right 	ASSIGMENT
%left	OR
%left	AND
%left	EQUAL NOT_EQUAL LT LE GT GE
%left	PLUS MINUS
%left	MULT DIV MOD
%left	NEG POS NOT
%right	POW


%%


// A program is a list of main elements (ATN's, states, functions, global variables)
prog 	: mainElement
		;

// A main element is an element as the type of: ATN, State, Function, Global variables
mainElement	: func mainElement
			| //nothing
			;

/*
mainElement	: atn
			| state
			| func
			| global
			| //nothing
			;
*/

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID params block_instructions
		;

// The list of parameters grouped in a subtree (it can be empty)
params	: OPARENTHESIS possible_params CPARENTHESIS
		;

possible_params	: param paramlist
				| //nothing
				;

// Parameters are separated by commas
paramlist	: COMMA param paramlist
			| //nothing
			;

// Parameters with & as prefix are passed by reference
// Only one node with the name of the parameter is created
param 	: ANDPERSAND ID
		| ID
		;
/*
	param   :   '&' id=ID -> ^(PREF[$id,$id.text])
    	    |   id=ID -> ^(PVALUE[$id,$id.text])
        	;
*/

// A list of instructions, all of them gouped in a subtree
block_instructions	: OBRACER instruction_list CBRACER
					;

instruction_list	: instruction SEMICOLON instruction_list
					| //nothing
					;

// The different types of instructions
instruction
		:	assign 			// Assignment
		|	ite_stmt		// if-then-else
		|	while_stmt		// while statement
		//|	for_stmt		// for statement
		//|	dowhile_stmt	// do while statement
		|	funcall			// Call to a procedure (no result produced)
		|	return_stmt		// Return statement
		|					// Nothing
		;

// Assignment
assign	: ID ASSIGMENT expr
		;

// if-then-else (else if & else are optionals)
ite_stmt	: IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if else
			;

// else-if (multiple and optional)
else_if	: else_if ELSE IF OPARENTHESIS expr CPARENTHESIS block_instructions
		| //nothing
		;

// else (optional)
else 	: ELSE block_instructions
		;

// while statement
while_stmt	: WHILE OPARENTHESIS expr CPARENTHESIS block_instructions
			;

// Return statement with an expression
return_stmt	: RETURN expr
			| RETURN
			;

expr: 
	BOOL
	{
		cout << "BOOL: " << $1 << endl;
	}
|
	INT
	{
		cout << "INT: " << $1 << endl;
	}
|
	DOUBLE
	{
	}
|
	STRING
	{
		cout << "STRING: " << $1 << endl;
	}
|
	ID
	{
		cout << "ID: " << $1 << endl;
		/*
			$$.p = static_cast<void*>(new std::string(d_scanner.matched()));
			void* vp = static_cast<void*>(new std::string(d_scanner.matched()));
			std::string* sp = static_cast<std::string*>(vp);
			std::string s = *sp;
			std::cout << "\t ID: " << s << '\n';
		*/
	}
|
	funcall
	{
		cout << "FUNCALL" << endl;
	}
|
	OPARENTHESIS expr CPARENTHESIS
	{
		cout << "PARENTESIS" << endl;
	}
|
	NOT expr
	{
		cout << "NOT" << endl;
	}
	|
	MINUS expr %prec NEG
	{
		cout << "MINUS" << endl;
	}
	|
	PLUS expr %prec POS
	{
	}
|
	expr PLUS expr
	{
		cout << "PLUS" << endl;
	}
|
	expr MINUS expr
	{
	}
|
	expr MULT expr
	{
		cout << "MULT" << endl;
	}
|
	expr DIV expr
	{
	}
|
	expr MOD expr
	{
	}
|
	expr POW expr
	{
		cout << "POW" << endl;
	}
|
	expr EQUAL expr
	{
		cout << "EQUAL" << endl;
	}
|
	expr NOT_EQUAL expr
	{
	}
|
	expr LT expr
	{
	}
|
	expr LE expr
	{
	}
|
	expr GT expr
	{
	}
|
	expr GE expr
	{
	}
|
	expr AND expr
	{
		cout << "AND" << endl;
	}
|
	expr OR expr
	{
	}
	;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall : ID OPARENTHESIS possible_exprs CPARENTHESIS
		;

possible_exprs 	: expr expr_list
				| //nothing
				;

// A list of expressions separated by commas
expr_list	: COMMA expr expr_list
			| //nothing
			;


%%


int main(int argc, char* argv[]) {
	// open a file handle to a particular file:
	if (argc > 1) {
		FILE *myfile = fopen(argv[1], "r");
		// make sure it's valid:
		if (!myfile) {
			cout << "I can't open " << argv[1] << "!" << endl;
			return -1;
		}
		// set lex to read from it instead of defaulting to STDIN:
		yyin = myfile;
	}
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