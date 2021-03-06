/* --Parser-- */

%scanner 					../lexer/Scanner.ih
%scanner-token-function 	d_scanner.lex()
%baseclass-preinclude		<iostream> 

%{
	#include "../utils/tree.h"
%}

%union
{
	int* t;
}

%token 	COMMA SEMICOLON COLON ANDPERSAND QUOTE OPARENTHESIS CPARENTHESIS OBRACKET CBRACKET OBRACER CBRACER
%token 	IF ELSE WHILE DO FOR FUNC RETURN GLOBAL
%token 	<t>	BOOL INT DOUBLE ID
%type 	<t> funcall expr

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
prog 	: mainElement EOF
		;

// A main element is an element as the type of: ATN, State, Function, Global variables
mainElement	: mainElement func
			| //nothing
			;

/*
mainElement	: mainElement atn
			| mainElement state
			| mainElement func
			| mainElement global
			| //nothing
			;
*/

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID params block_instructions
		;


// The list of parameters grouped in a subtree (it can be empty)
params	: OPARENTHESIS paramlist CPARENTHESIS
		;

// Parameters are separated by commas
paramlist	: param multiple_params
			| //nothing
			;

// Because in Bisonc++ doesn't exist the '*' operator
multiple_params	: multiple_params COMMA param
				| //nothing
				;

// Parameters with & as prefix are passed by reference
// Only one node with the name of the parameter is created
param 	: ANDPERSAND ID
		;
/*
	param   :   '&' id=ID -> ^(PREF[$id,$id.text])
    	    |   id=ID -> ^(PVALUE[$id,$id.text])
        	;
*/

// A list of instructions, all of them gouped in a subtree
block_instructions	: OBRACER instruction multiple_instrucitons CBRACER
					;

// Because in Bisonc++ doesn't exist the '*' operator
multiple_instrucitons	: multiple_instrucitons SEMICOLON instruction
						| //nothing
						;

// The different types of instructions
instruction
		:	assign 		// Assignment
		|	ite_stmt		// if-then-else
		|	while_stmt		// while statement
		//|	for_stmt		// for statement
		//|	dowhile_stmt	// do while statement
		|	funcall			// Call to a procedure (no result produced)
		|	return_stmt		// Return statement
		|					// Nothing
		;

// Assignment
assign	:	ID ASSIGMENT expr
		;

// if-then-else (else if & else are optionals)
ite_stmt	:	IF expr block_instructions else_if else
			;

// else-if (multiple and optional)
else_if	: else_if ELSE IF expr block_instructions
		| //nothing
		;

// else (optional)
else 	: ELSE block_instructions
		| //nothing
		;

// while statement
while_stmt	:	WHILE expr block_instructions
			;

// Return statement with an expression
return_stmt	:	RETURN possible_expr
			;

possible_expr	: expr
				| //nothing
				;

expr:
	BOOL
	{
	}
|
	INT
	{
//		$$.i = stoi(d_scanner.matched());
//		std::cout << "\t INT: " << $$.i << '\n';
	}
|
	DOUBLE
	{
//		$$.d = stod(d_scanner.matched());
//		std::cout << "\t DOUBLE: " << $$.d << '\n';
	}
|
	ID
	{
/*		$$.p = static_cast<void*>(new std::string(d_scanner.matched()));
		void* vp = static_cast<void*>(new std::string(d_scanner.matched()));
		std::string* sp = static_cast<std::string*>(vp);
		std::string s = *sp;
		std::cout << "\t ID: " << s << '\n';
*/	}
|
	funcall
	{
	}
|
	OPARENTHESIS expr CPARENTHESIS
	{
	}
|
	NOT expr
	{
	}
|
	MINUS expr %prec NEG
	{
//		$$ = -$2;
//		std::cout << "\t NEG: " << $2 << " = " << $$ << '\n';
	}
|
	PLUS expr %prec POS
	{
	}
|
	expr PLUS expr
	{
//		$$ = $1 + $3;
//		std::cout << "\t PLUS: " << $1 << " + " << $3 << " = " << $$ << '\n';
	}
|
	expr MINUS expr
	{
	}
|
	expr MULT expr
	{
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
	}
|
	expr EQUAL expr
	{
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
	}
|
	expr OR expr
	{
	}
;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall : ID OPARENTHESIS expr_list CPARENTHESIS
		;

// A list of expressions separated by commas
expr_list	: expr multiple_expr
			| //nothing
			;

// Because in Bisonc++ doesn't exist the '*' operator
multiple_expr	: multiple_expr COMMA expr
				| //nothing
				;


/*
//	COMMENT	: '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
//	   		| '/*' ( options {greedy=false;} : . )* '*///' {$channel=HIDDEN;}
//		;

	// Strings (in quotes) with escape sequences        
/*	STRING  :  '"' ( ESC_SEQ | ~('\\'|'"') )* '"'
//	        ;

	fragment
	ESC_SEQ
	    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
	    ;
*/