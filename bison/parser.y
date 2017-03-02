%skeleton "lalr1.cc" /* C++ */
%require "3.0"
%defines
%define parser_class_name { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { ATN }
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include "tree.h"
    #include "ASTN.h"

    using namespace std;
    using namespace freeling;

    namespace ATN {
        class Scanner;
        class Interpreter;
    }
}

// This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// This function is called only inside Bison, so we make it static to limit
// symbol visibility for the linker to avoid potential linking conflicts.
%code top
{
    #include <iostream>
    #include "scanner.h"
    #include "parser.hpp"
    #include "interpreter.h"
    #include "location.hh"
    
    // yylex() arguments are defined in parser.y
    static ATN::Parser::symbol_type yylex(ATN::Scanner &scanner, ATN::Interpreter &driver) {
        return scanner.get_next_token();
    }
    
    using namespace ATN;
}


%lex-param { ATN::Scanner &scanner }
%lex-param { ATN::Interpreter &driver }
%parse-param { ATN::Scanner &scanner }
%parse-param { ATN::Interpreter &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token DOT "dot";
%token COMMA "comma";
%token SEMICOLON "semicolon";
%token COLON "colon";
%token ANDPERSAND "andpersand";
%token OPARENTHESIS "open parenthesis";
%token CPARENTHESIS "close parenthesis";
%token OBRACKET "open bracket";
%token CBRACKET "close bracket";
%token OBRACER "open bracer";
%token CBRACER "close bracer";
%token NOT_DEFINED "not defined";

%token PRINT "print";
%token IF "if";
%token ELSE "else";
%token WHILE "while";
%token DO "do";
%token FOR "for";
%token FUNC "function";
%token RETURN "return";
%token ADD "add";
%token REMOVE "remove";
%token INDEXOF "indexof";
%token SIZE "size";
%token GLOBAL "global";
%token ATN "atn";
%token STATE "state";
%token INITIAL "initial states";
%token FINAL "final states";
%token ACTION "action";
%token TRANSITION "transition";
%token TO "to";

%token <std::string> BOOL "bool";
%token <int> INT "int";
%token <float> DOUBLE "double";
%token <std::string> STRING "string";
%token <std::string> ID "id";

%right 	ASSIGMENT
%left	OR
%left	AND
%left	EQUAL NOT_EQUAL LT LE GT GE
%left	PLUS MINUS DPLUS DMINUS
%left	MULT DIV MOD
%left	NEG POS NOT
%right	POW

%type < freeling::tree<ATN::ASTN>* > mainElement global func param_list param block_instructions;
%type < freeling::tree<ATN::ASTN>* > atn global_list initials finals id_list states state transition_list;
%type < freeling::tree<ATN::ASTN>* > instruction_list instruction assign ite_stmt else_if else double_arithmetic;
%type < freeling::tree<ATN::ASTN>* > while_stmt for_stmt incremental dowhile_stmt return_stmt print_stmt expr funcall expr_list object_list;

%start program


%%


// A program is a list of main elements (ATN's, states, functions, global variables)
program 	: { driver.clear(); }
			| program mainElement
				{
					tree<ASTN>* t = $2;
					tree<ASTN>::const_iterator it = t->begin();
					ASTN node = *it;
					wstring token = node.getToken();

					if (token == L"FUNCTION" || token == L"ATN") {
						driver.addMainElement(node.getValueWstring(), *t);
					}
					else { // token == L"GLOBAL"
						for (int i = 0; i < (*t).num_children(); ++i) {
							tree<ASTN> tr = it.nth_child_ref(i);
							tree<ASTN>::const_iterator aux = tr.begin();
							node = *aux;
							driver.addMainElement(node.getValueWstring(), tr);
						}
					}					
				}
		;

// A main element is an element as the type of: ATN, Function, Global variables
mainElement	: global SEMICOLON 	{ $$ = $1; }
			| func   			{ $$ = $1; }
			| atn 	 			{ $$ = $1; }
			;

// A global variable
global 	: GLOBAL ID 
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"GLOBAL"));
				tree<ASTN>* id = new tree<ASTN>(ASTN(L"GLOBAL ID", wstring($2.begin(), $2.end())));

				(*t).add_child(*id);
				$$ = t;
			}
		| global COMMA ID
			{
				tree<ASTN>* t = $1;
				tree<ASTN>* id = new tree<ASTN>(ASTN(L"GLOBAL ID", wstring($3.begin(), $3.end())));

				(*t).add_child(*id);
				$$ = t;
			}
		;

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID OPARENTHESIS param_list CPARENTHESIS block_instructions
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"FUNCTION", wstring($2.begin(), $2.end())));
				const tree<ASTN>* params = $4;
				const tree<ASTN>* body = $6;

				(*t).add_child(*params);
				(*t).add_child(*body);
				$$ = t;
			}
		;

// The list of parameters grouped in a subtree (it can be empty)
param_list	: // nothing
				{
					$$ = new tree<ASTN>(ASTN(L"PARAM LIST"));
				}
			| param
				{
					tree<ASTN>* params = new tree<ASTN>(ASTN(L"PARAM LIST"));
					tree<ASTN>* p = $1;

					(*params).add_child(*p);
					$$ = params;
				}
			| param_list COMMA param
				{
					tree<ASTN>* params = $1;
					tree<ASTN>* p = $3;

					(*params).add_child(*p);
					$$ = params;
				}
			;

// Parameters with & as prefix are passed by reference
// Only one node with the name of the parameter is created
param 	: ANDPERSAND ID
			{
				$$ = new tree<ASTN>(ASTN(L"ANDPERSAND", wstring($2.begin(), $2.end())));
			}
		| ID
			{
				$$ = new tree<ASTN>(ASTN(L"VALUE", wstring($1.begin(), $1.end())));
			}
		;

// ATN
atn 	: ATN ID OBRACER global_list initials finals states CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ATN", wstring($2.begin(), $2.end())));
				tree<ASTN>* init = $4;
				tree<ASTN>* fin = $5;
				tree<ASTN>* sts = $6;

				(*t).add_child(*init);
				(*t).add_child(*fin);
				(*t).add_child(*sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list initials states finals CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ATN", wstring($2.begin(), $2.end())));
				tree<ASTN>* init = $4;
				tree<ASTN>* fin = $6;
				tree<ASTN>* sts = $5;

				(*t).add_child(*init);
				(*t).add_child(*fin);
				(*t).add_child(*sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list finals initials states CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ATN", wstring($2.begin(), $2.end())));
				tree<ASTN>* init = $5;
				tree<ASTN>* fin = $4;
				tree<ASTN>* sts = $6;

				(*t).add_child(*init);
				(*t).add_child(*fin);
				(*t).add_child(*sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list finals states initials CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ATN", wstring($2.begin(), $2.end())));
				tree<ASTN>* init = $6;
				tree<ASTN>* fin = $4;
				tree<ASTN>* sts = $5;

				(*t).add_child(*init);
				(*t).add_child(*fin);
				(*t).add_child(*sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list states initials finals CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ATN", wstring($2.begin(), $2.end())));
				tree<ASTN>* init = $5;
				tree<ASTN>* fin = $6;
				tree<ASTN>* sts = $4;

				(*t).add_child(*init);
				(*t).add_child(*fin);
				(*t).add_child(*sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list states finals initials CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ATN", wstring($2.begin(), $2.end())));
				tree<ASTN>* init = $6;
				tree<ASTN>* fin = $5;
				tree<ASTN>* sts = $4;

				(*t).add_child(*init);
				(*t).add_child(*fin);
				(*t).add_child(*sts);
				$$ = t;
			}
		;

// List of global
global_list 	: //nothing
					{
						$$ = new tree<ASTN>(ASTN(L"GLOBAL LIST"));
					}
				| global SEMICOLON
					{
						tree<ASTN>* t = new tree<ASTN>(ASTN(L"GLOBAL LIST"));
						(*t).add_child(*$1);
						$$ = t;
					}
				| global_list global SEMICOLON
					{
						tree<ASTN>* t = $1;
						(*t).add_child(*$2);
						$$ = t;
					}
				;

// The initials states of an ATN
initials 	: INITIAL COLON id_list SEMICOLON
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"INITIALS"));
					tree<ASTN>* list = $3;

					(*t).add_child(*$3);
					$$ = t;
				}
			;

// The finals states of an ATN
finals 	: FINAL COLON id_list SEMICOLON
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"FINALS"));
					tree<ASTN>* list = $3;

					(*t).add_child(*$3);
					$$ = t;
				}
			;

// List of ids
id_list	: ID
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ID LIST"));
				tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));

				(*t).add_child(*id);
				$$ = t;
			}
		| id_list COMMA ID
			{
				tree<ASTN>* t = $1;
				tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($3.begin(), $3.end())));

				(*t).add_child(*id);
				$$ = t;
			}
		;

// States of the ATN
states 	: state
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"STATES"));
				(*t).add_child(*$1);
				$$ = t;
			}
		| states state
			{
				tree<ASTN>* t = $1;
				(*t).add_child(*$2);
				$$ = $1;
			}
		;

// Single state
state 	: STATE ID OBRACER ACTION block_instructions TRANSITION OBRACER transition_list CBRACER CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"STATE", wstring($2.begin(), $2.end())));
				
				tree<ASTN>* act = new tree<ASTN>(ASTN(L"ACTION"));
				tree<ASTN>* instr = $5;
				(*act).add_child(*instr);

				tree<ASTN>* trans = $8;

				(*t).add_child(*act);
				(*t).add_child(*trans);
				$$ = t;
			}
		| STATE ID OBRACER TRANSITION OBRACER transition_list CBRACER ACTION block_instructions CBRACER
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"STATE", wstring($2.begin(), $2.end())));
				
				tree<ASTN>* act = new tree<ASTN>(ASTN(L"ACTION"));
				tree<ASTN>* instr = $9;
				(*act).add_child(*instr);

				tree<ASTN>* trans = $6;

				(*t).add_child(*act);
				(*t).add_child(*trans);
				$$ = t;
			}
		;

// List of transitions
transition_list 	: // nothing
						{
							$$ = new tree<ASTN>(ASTN(L"TRANSITION"));
						}
					| TO ID IF OPARENTHESIS expr CPARENTHESIS SEMICOLON
						{
							tree<ASTN>* t = new tree<ASTN>(ASTN(L"TRANSITION"));
							tree<ASTN>* trans = new tree<ASTN>(ASTN(L"TO"));
							tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($2.begin(), $2.end())));
							tree<ASTN>* cond = $5;

							(*trans).add_child(*id);
							(*trans).add_child(*cond);
							(*t).add_child(*trans);
							$$ = t;
						}
					| transition_list TO ID IF OPARENTHESIS expr CPARENTHESIS SEMICOLON
						{
							tree<ASTN>* t = $1;
							tree<ASTN>* trans = new tree<ASTN>(ASTN(L"TO"));
							tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($3.begin(), $3.end())));
							tree<ASTN>* cond = $6;

							(*trans).add_child(*id);
							(*trans).add_child(*cond);
							(*t).add_child(*trans);
							$$ = t;
						}
					;

// A list of instructions, all of them gouped in a subtree
block_instructions	: OBRACER instruction_list CBRACER
						{
							$$ = $2;
						}
					;

// The list of instructions
instruction_list	: // nothing
						{
							$$ = new tree<ASTN>(ASTN(L"BODY"));
						}
					| instruction
						{
							tree<ASTN>* body = new tree<ASTN>(ASTN(L"BODY"));
							tree<ASTN>* instruction = $1;

							(*body).add_child(*instruction);
							$$ = body;
						}
					| instruction_list instruction
						{
							tree<ASTN>* body = $1;
							tree<ASTN>* instruction = $2;

							(*body).add_child(*instruction);
							$$ = body;
						}
					;

// The different types of instructions
instruction
		:	assign SEMICOLON 				// Assignment
				{ $$ = $1; }
		|	ite_stmt						// if-then-else
				{ $$ = $1; }
		|	while_stmt						// while statement
				{ $$ = $1; }
		|	for_stmt						// for statement
				{ $$ = $1; }
		|	dowhile_stmt					// do while statement
				{ $$ = $1; }
		|	funcall	SEMICOLON				// Call to a procedure (no result produced)
				{ $$ = $1; }
		|	return_stmt	SEMICOLON 			// Return statement
				{ $$ = $1; }
		|	double_arithmetic SEMICOLON 	// Double plus or double minus 
				{ $$ = $1; }
		|	print_stmt SEMICOLON 			// Print statement
				{ $$ = $1; }
		;

// Assignment
assign	: expr ASSIGMENT expr
			{
				tree<ASTN>* instr = new tree<ASTN>(ASTN(L"ASSIGMENT"));
				tree<ASTN>* id = $1;
				tree<ASTN>* body = $3;

				(*instr).add_child(*id);
				(*instr).add_child(*body);
				$$ = instr;
			}
		;

// if-then-else (else if & else are optionals)
ite_stmt	: IF OPARENTHESIS expr CPARENTHESIS block_instructions
				{
					tree<ASTN>* ite = new tree<ASTN>(ASTN(L"IF"));
					tree<ASTN>* condition = $3;
					tree<ASTN>* body = $5;

					(*ite).add_child(*condition);
					(*ite).add_child(*body);
					$$ = ite;
				}
			| IF OPARENTHESIS expr CPARENTHESIS block_instructions else
				{
					tree<ASTN>* ite = new tree<ASTN>(ASTN(L"IF ELSE"));
					tree<ASTN>* condition = $3;
					tree<ASTN>* body = $5;
					tree<ASTN>* else_tree = $6;

					(*ite).add_child(*condition);
					(*ite).add_child(*body);
					(*ite).add_child(*else_tree);
					$$ = ite;
				}
			| IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if
				{
					tree<ASTN>* ite = new tree<ASTN>(ASTN(L"IF ELSE IF"));
					tree<ASTN>* condition = $3;
					tree<ASTN>* body = $5;
					tree<ASTN>* elseif = $6;

					(*ite).add_child(*condition);
					(*ite).add_child(*body);
					(*ite).add_child(*elseif);
					$$ = ite;
				}
			| IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if else
				{
					tree<ASTN>* ite = new tree<ASTN>(ASTN(L"IF ELSE IF ELSE"));
					tree<ASTN>* condition = $3;
					tree<ASTN>* body = $5;
					tree<ASTN>* elseif = $6;
					tree<ASTN>* else_tree = $7;

					(*ite).add_child(*condition);
					(*ite).add_child(*body);
					(*ite).add_child(*elseif);
					(*ite).add_child(*else_tree);
					$$ = ite;
				}
			;

// else-if
else_if	: ELSE IF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ASTN>* elif = new tree<ASTN>(ASTN(L"ELSE IF"));
				tree<ASTN>* condition = $4;
				tree<ASTN>* body = $6;

				(*elif).add_child(*condition);
				(*elif).add_child(*body);
				$$ = elif;
			}
		| else_if ELSE IF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ASTN>* elif = $1;
				tree<ASTN>* condition = $5;
				tree<ASTN>* body = $7;

				(*elif).add_child(*condition);
				(*elif).add_child(*body);
				$$ = elif;
			}
		;

// else
else 	: ELSE block_instructions
			{
				tree<ASTN>* t = new tree<ASTN>(ASTN(L"ELSE"));
				tree<ASTN>* body = $2;

				(*t).add_child(*body);
				$$ = t;
			}
		;

// while statement
while_stmt	: WHILE OPARENTHESIS expr CPARENTHESIS block_instructions
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"WHILE"));
					tree<ASTN>* condition = $3;
					tree<ASTN>* body = $5;

					(*t).add_child(*condition);
					(*t).add_child(*body);
					$$ = t;
				}
			;

// for statement
for_stmt	: FOR OPARENTHESIS assign SEMICOLON expr SEMICOLON incremental CPARENTHESIS block_instructions
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"FOR"));
					tree<ASTN>* init = $3;
					tree<ASTN>* condition = $5;
					tree<ASTN>* final = $7;
					tree<ASTN>* body = $9;

					(*t).add_child(*init);
					(*t).add_child(*condition);
					(*t).add_child(*final);
					(*t).add_child(*body);
					$$ = t;
				}
			| FOR OPARENTHESIS expr SEMICOLON instruction CPARENTHESIS block_instructions
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"FOR"));
					tree<ASTN>* condition = $3;
					tree<ASTN>* final = $5;
					tree<ASTN>* body = $7;

					(*t).add_child(*condition);
					(*t).add_child(*final);
					(*t).add_child(*body);
					$$ = t;
				}
			;

// assigment or expr
incremental 	: assign
					{ $$ = $1; }
				| double_arithmetic
					{ $$ = $1; }
				;

// dowhile statement
dowhile_stmt	: DO block_instructions WHILE OPARENTHESIS expr CPARENTHESIS
				{
					tree<ASTN>* dowhile = new tree<ASTN>(ASTN(L"DO WHILE"));
					tree<ASTN>* body = $2;
					tree<ASTN>* condition = $5;

					(*dowhile).add_child(*body);
					(*dowhile).add_child(*condition);
					$$ = dowhile;
				}

// Return statement with an expression
return_stmt	: RETURN
				{
					$$ = new tree<ASTN>(ASTN(L"RETURN"));
				}
			| RETURN expr
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"RETURN"));
					tree<ASTN>* expr = $2;

					(*t).add_child(*expr);
					$$ = t;
				}
			;

// Print statement
print_stmt 	: PRINT expr
				{
					tree<ASTN>* t = new tree<ASTN>(ASTN(L"PRINT"));
					tree<ASTN>* e = $2;

					(*t).add_child(*e);
					$$ = t;
				}
			;

// Simple expression
expr: BOOL
		{
			bool b = $1 == "true" || $1 == "TRUE";
			$$ = new tree<ASTN>(ASTN(L"TOKEN BOOL", b));
		}
	| INT
		{
			$$ = new tree<ASTN>(ASTN(L"TOKEN INT", $1));
		}
	| DOUBLE
		{
			$$ = new tree<ASTN>(ASTN(L"TOKEN DOUBLE", $1));
		}
	| STRING
		{
			$$ = new tree<ASTN>(ASTN(L"TOKEN STRING", wstring($1.begin(), $1.end())));
		}
	| ID
		{
			$$ = new tree<ASTN>(ASTN(L"TOKEN ID", wstring($1.begin(), $1.end())));
		}
	| OBRACER CBRACER
		{
			$$ = new tree<ASTN>(ASTN(L"TOKEN OBJECT"));
		}
	| OBRACER object_list CBRACER
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"TOKEN OBJECT"));
			tree<ASTN>* list = $2;

			(*n).add_child(*list);
			$$ = n;
		}
	| OBRACKET CBRACKET
		{
			$$ = new tree<ASTN>(ASTN(L"TOKEN ARRAY"));
		}
	| OBRACKET expr_list CBRACKET
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"TOKEN ARRAY"));
			tree<ASTN>* list = $2;

			(*n).add_child(*list);
			$$ = n;
		}
	| funcall
		{
			$$ = $1;
		}
	| OPARENTHESIS expr CPARENTHESIS
		{
			$$ = $2;
		}
	| NOT expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"NOT"));
			tree<ASTN>* expr = $2;

			(*n).add_child(*expr);
			$$ = n;
		}
	| MINUS expr %prec NEG
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"NEG"));
			tree<ASTN>* expr = $2;

			(*n).add_child(*expr);
			$$ = n;
		}
	| PLUS expr %prec POS
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"POS"));
			tree<ASTN>* expr = $2;

			(*n).add_child(*expr);
			$$ = n;
		}
	| double_arithmetic
		{ $$ = $1; }
	| expr PLUS expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"PLUS"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr MINUS expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"MINUS"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr MULT expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"MULT"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr DIV expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"DIV"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr MOD expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"MOD"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr EQUAL expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"EQUAL"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr NOT_EQUAL expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"NOT_EQUAL"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr LT expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"LT"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr LE expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"LE"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr GT expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"GT"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr GE expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"GE"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr AND expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"AND"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| expr OR expr
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"OR"));
			tree<ASTN>* expr1 = $1;
			tree<ASTN>* expr2 = $3;

			(*n).add_child(*expr1);
			(*n).add_child(*expr2);
			$$ = n;
		}
	| ID DOT ID
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"OBJECT ACCES"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
			tree<ASTN>* pos = new tree<ASTN>(ASTN(L"POSITION", wstring($3.begin(), $3.end())));

			(*n).add_child(*id);
			(*n).add_child(*pos);
			$$ = n;
		}
	| ID OBRACKET expr CBRACKET
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"ARRAY ACCES"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
			tree<ASTN>* pos = $3;

			(*n).add_child(*id);
			(*n).add_child(*pos);
			$$ = n;
		}
	| ID DOT ADD OPARENTHESIS expr CPARENTHESIS
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"ARRAY ADD"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
			tree<ASTN>* element = $5;

			(*n).add_child(*id);
			(*n).add_child(*element);
			$$ = n;
		}
	| ID DOT ADD OPARENTHESIS expr COMMA expr CPARENTHESIS
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"ARRAY ADD"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
			tree<ASTN>* element = $7;
			tree<ASTN>* pos = $5;

			(*n).add_child(*id);
			(*n).add_child(*element);
			(*n).add_child(*pos);
			$$ = n;
		}
	| ID DOT REMOVE OPARENTHESIS expr CPARENTHESIS
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"ARRAY REMOVE"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
			tree<ASTN>* pos = $5;

			(*n).add_child(*id);
			(*n).add_child(*pos);
			$$ = n;
		}
	| ID DOT INDEXOF OPARENTHESIS expr CPARENTHESIS
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"ARRAY INDEXOF"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
			tree<ASTN>* element = $5;

			(*n).add_child(*id);
			(*n).add_child(*element);
			$$ = n;
		}
	| ID DOT SIZE OPARENTHESIS CPARENTHESIS
		{
			tree<ASTN>* n = new tree<ASTN>(ASTN(L"ARRAY SIZE"));
			tree<ASTN>* id = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));

			(*n).add_child(*id);
			$$ = n;
		}
	;

// double plus or double minus
double_arithmetic 	: DPLUS expr
						{
							tree<ASTN>* n = new tree<ASTN>(ASTN(L"DPLUS"));
							tree<ASTN>* e = $2;

							(*n).add_child(*e);
							$$ = n;
						}
					| expr DPLUS
						{
							tree<ASTN>* n = new tree<ASTN>(ASTN(L"DPLUSR"));
							tree<ASTN>* e = $1;

							(*n).add_child(*e);
							$$ = n;
						}
					| DMINUS expr
						{
							tree<ASTN>* n = new tree<ASTN>(ASTN(L"DMINUS"));
							tree<ASTN>* e = $2;

							(*n).add_child(*e);
							$$ = n;
						}
					| expr DMINUS
						{
							tree<ASTN>* n = new tree<ASTN>(ASTN(L"DMINUSR"));
							tree<ASTN>* e = $1;

							(*n).add_child(*e);
							$$ = n;
						}
					;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall : ID OPARENTHESIS expr_list CPARENTHESIS
			{
				tree<ASTN>* funcall = new tree<ASTN>(ASTN(L"FUNCALL", wstring($1.begin(), $1.end())));
				tree<ASTN>* list = $3;

				(*funcall).add_child(*list);
				$$ = funcall;
			}
		;

// A list of expressions separated by commas
expr_list	: // nothing
				{
					$$ = new tree<ASTN>(ASTN(L"EXPR LIST"));
				}
			| expr
				{
					tree<ASTN>* list = new tree<ASTN>(ASTN(L"EXPR LIST"));
					tree<ASTN>* expr = $1;

					(*list).add_child(*expr);
					$$ = list;
				}
			| expr_list COMMA expr
				{
					tree<ASTN>* list = $1;
					tree<ASTN>* expr = $3;

					(*list).add_child(*expr);
					$$ = list;
				}
			;

// A list of declarations of the items of a dictionary
object_list 	: ID COLON expr
					{
						tree<ASTN>* t = new tree<ASTN>(ASTN(L"OBJECT LIST"));
						tree<ASTN>* obj = new tree<ASTN>(ASTN(L"ID", wstring($1.begin(), $1.end())));
						tree<ASTN>* body = $3;

						(*obj).add_child(*body);
						(*t).add_child(*obj);
						$$ = t;
					}
				| object_list ID COLON expr
					{
						tree<ASTN>* t = $1;
						tree<ASTN>* obj = new tree<ASTN>(ASTN(L"ID", wstring($2.begin(), $2.end())));
						tree<ASTN>* body = $4;

						(*obj).add_child(*body);
						(*t).add_child(*obj);
						$$ = t;
					}
				;


%%


// Bison expects us to provide implementation - otherwise linker complains
void ATN::Parser::error(const location &loc , const std::string &message) {
    cout << "Error: " << message << endl << "Error location: " << driver.location() << endl << "Row: " << driver.row() << "  Column: " << driver.column() << endl;
}