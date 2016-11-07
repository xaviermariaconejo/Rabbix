/* --Parser-- */

%scanner 					../lexer/Scanner.ih
%scanner-token-function 	d_scanner.lex()

%union
{
	tree<node>* t;
}

//%token 					OPARENTHESIS CPARENTHESIS EOF
%token 	<t>				BOOL INT DOUBLE ID funcall exp

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
prog 	: mainElement+ EOF
		;

// A main element is an element as the type of: ATN, State, Function, Global variables
mainElement	: //atn
		//	| state
			| func
		//	| global

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID params block_instructions
		;


// The list of parameters grouped in a subtree (it can be empty)
params	: OPARENTHESIS paramlist? CPARENTHESIS
		;

// Parameters are separated by commas
paramlist	: param (COMMA param)*
			;

// Parameters with & as prefix are passed by reference
// Only one node with the name of the parameter is created
param 	:	ANDPERSAND ID
		;
/*
	param   :   '&' id=ID -> ^(PREF[$id,$id.text])
    	    |   id=ID -> ^(PVALUE[$id,$id.text])
        	;
*/

// A list of instructions, all of them gouped in a subtree
block_instructions	:	OBRACER instruction (SEMICOLON instruction)* CBRACER
					;

// The different types of instructions
instruction
		:	Assignment 		// Assignment
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

// if-then-else (else is optional)
ite_stmt	:	IF expr block_instructions (ELSE IF expr block_instructions)* (ELSE block_instructions)?
			;

// while statement
while_stmt	:	WHILE expr block_instructions
			;

// Return statement with an expression
return_stmt	:	RETURN expr?
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
	expr (PLUS | MINUS | MULT | MULT | DIV | MOD | POW) expr
	{
//		$$ = $1 + $3;
//		std::cout << "\t PLUS: " << $1 << " + " << $3 << " = " << $$ << '\n';
	}
|
	expr (EQUAL | NOT_EQUAL | LT | LE | GT | GE | AND | OR) expr
	{
	}
;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall :	ID OPARENTHESIS expr_list? CPARENTHESIS
		;

// A list of expressions separated by commas
expr_list	:	expr (COMMA expr)*
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