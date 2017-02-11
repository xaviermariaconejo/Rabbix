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
%left	PLUS MINUS DOUBLEPLUS DOUBLEMINUS
%left	MULT DIV MOD
%left	NEG POS NOT
%right	POW

%type < freeling::tree<ATN::ASTN> > mainElement global func params param_list param block_instructions;
%type < freeling::tree<ATN::ASTN> > atn global_list function_list initials finals id_list states state transition_list;
%type < freeling::tree<ATN::ASTN> > instruction_list instruction assign ite_stmt else_if else;
%type < freeling::tree<ATN::ASTN> > while_stmt for_stmt dowhile_stmt return_stmt expr funcall expr_list object_list;

%start program


%%


// A program is a list of main elements (ATN's, states, functions, global variables)
program 	: { cout << "INIT FIRST CASE" << endl; driver.clear(); }
			| program mainElement
				{
					cout << "INIT SECOND CASE" << endl;
					const tree<ASTN>& t = $2;
					driver.addMainElement(t);
				}
		;

// A main element is an element as the type of: ATN, Function, Global variables
mainElement	: global
			| func
			| atn
			;

// A global variable
global 	: GLOBAL ID SEMICOLON
			{
				$$ = tree<ASTN> (ASTN(L"GLOBAL", wstring($2.begin(), $2.end())));
			}
		;

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID params block_instructions
			{
				tree<ASTN> t(ASTN(L"FUNCTION",wstring($2.begin(), $2.end())));
				const tree<ASTN>& params = $3;
				const tree<ASTN>& body = $4;

				t.add_child(params);
				t.add_child(body);
				$$ = t;
			}
		;

// The list of parameters grouped in a subtree (it can be empty)
params	: OPARENTHESIS CPARENTHESIS
			{
				$$ = tree<ASTN> (ASTN(L"PARAMS"));
			}
		| OPARENTHESIS param_list CPARENTHESIS
			{
				$$ = $2;
			}
		;

// Parameters are separated by commas
param_list	: param
				{
					tree<ASTN> params(ASTN(L"PARAM LIST"));
					const tree<ASTN>& p = $1;

					params.add_child(p);
					$$ = params;
				}
			| param_list COMMA param
				{
					tree<ASTN>& params = $1;
					const tree<ASTN>& p = $3;

					params.add_child(p);
					$$ = params;
				}
			;

// Parameters with & as prefix are passed by reference
// Only one node with the name of the parameter is created
param 	: ANDPERSAND ID
			{
				$$ = tree<ASTN> (ASTN(L"ANDPERSAND", wstring($2.begin(), $2.end())));
			}
		| ID
			{
				$$ = tree<ASTN> (ASTN(L"VALUE", wstring($1.begin(), $1.end())));
			}
		;

// ATN
atn 	: ATN ID OBRACER global_list function_list initials finals states CBRACER
			{
				tree<ASTN> t(ASTN(L"ATN",wstring($2.begin(), $2.end())));
				const tree<ASTN>& gls = $4;
				const tree<ASTN>& funcs = $5;
				const tree<ASTN>& init = $6;
				const tree<ASTN>& fin = $7;
				const tree<ASTN>& sts = $8;

				t.add_child(gls);
				t.add_child(funcs);
				t.add_child(init);
				t.add_child(fin);
				t.add_child(sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list function_list initials states finals CBRACER
			{
				tree<ASTN> t(ASTN(L"ATN",wstring($2.begin(), $2.end())));
				const tree<ASTN>& gls = $4;
				const tree<ASTN>& funcs = $5;
				const tree<ASTN>& init = $6;
				const tree<ASTN>& fin = $8;
				const tree<ASTN>& sts = $7;

				t.add_child(gls);
				t.add_child(funcs);
				t.add_child(init);
				t.add_child(fin);
				t.add_child(sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list function_list finals initials states CBRACER
			{
				tree<ASTN> t(ASTN(L"ATN",wstring($2.begin(), $2.end())));
				const tree<ASTN>& gls = $4;
				const tree<ASTN>& funcs = $5;
				const tree<ASTN>& init = $7;
				const tree<ASTN>& fin = $6;
				const tree<ASTN>& sts = $8;

				t.add_child(gls);
				t.add_child(funcs);
				t.add_child(init);
				t.add_child(fin);
				t.add_child(sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list function_list finals states initials CBRACER
			{
				tree<ASTN> t(ASTN(L"ATN",wstring($2.begin(), $2.end())));
				const tree<ASTN>& gls = $4;
				const tree<ASTN>& funcs = $5;
				const tree<ASTN>& init = $8;
				const tree<ASTN>& fin = $6;
				const tree<ASTN>& sts = $7;

				t.add_child(gls);
				t.add_child(funcs);
				t.add_child(init);
				t.add_child(fin);
				t.add_child(sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list function_list states initials finals CBRACER
			{
				tree<ASTN> t(ASTN(L"ATN",wstring($2.begin(), $2.end())));
				const tree<ASTN>& gls = $4;
				const tree<ASTN>& funcs = $5;
				const tree<ASTN>& init = $7;
				const tree<ASTN>& fin = $8;
				const tree<ASTN>& sts = $6;

				t.add_child(gls);
				t.add_child(funcs);
				t.add_child(init);
				t.add_child(fin);
				t.add_child(sts);
				$$ = t;
			}
		| ATN ID OBRACER global_list function_list states finals initials CBRACER
			{
				tree<ASTN> t(ASTN(L"ATN",wstring($2.begin(), $2.end())));
				const tree<ASTN>& gls = $4;
				const tree<ASTN>& funcs = $5;
				const tree<ASTN>& init = $8;
				const tree<ASTN>& fin = $7;
				const tree<ASTN>& sts = $6;

				t.add_child(gls);
				t.add_child(funcs);
				t.add_child(init);
				t.add_child(fin);
				t.add_child(sts);
				$$ = t;
			}
		;

// List of global
global_list 	: global
					{
						tree<ASTN> t(ASTN(L"GLOBAL LIST"));
						t.add_child($1);
						$$ = t;
					}
				| global_list global
					{
						$1.add_child($2);
						$$ = $1;
					}
				;

// List of functions
function_list 	: func
					{
						tree<ASTN> t(ASTN(L"FUNCTION LIST"));
						t.add_child($1);
						$$ = t;
					}
				| function_list func
					{
						$1.add_child($2);
						$$ = $1;
					}
				;

// The initials states of an ATN
initials 	: INITIAL COLON id_list SEMICOLON
				{
					tree<ASTN> t(ASTN(L"INITIALS"));
					const tree<ASTN> list = $3;

					t.add_child($3);
					$$ = t;
				}
			;

// The finals states of an ATN
finals 	: FINAL COLON id_list SEMICOLON
				{
					tree<ASTN> t(ASTN(L"FINALS"));
					const tree<ASTN> list = $3;

					t.add_child($3);
					$$ = t;
				}
			;

// List of ids
id_list	: ID
			{
				tree<ASTN> t(ASTN(L"ID LIST"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));

				t.add_child(id);
				$$ = t;
			}
		| id_list COMMA ID
			{
				tree<ASTN> t = $1;
				tree<ASTN> id(ASTN(L"ID", wstring($3.begin(), $3.end())));

				t.add_child(id);
				$$ = t;
			}
		;

// States of the ATN
states 	: state SEMICOLON
			{
				tree<ASTN> t(ASTN(L"STATES"));
				t.add_child($1);
				$$ = t;
			}
		| states state SEMICOLON
			{
				$1.add_child($2);
				$$ = $1;
			}
		;

// Single state
state 	: STATE ID OBRACER function_list ACTION block_instructions TRANSITION OBRACER CBRACER CBRACER
			{
				tree<ASTN> t(ASTN(L"STATE", wstring($2.begin(), $2.end())));
				
				const tree<ASTN>& funcs = $4;

				tree<ASTN> act(ASTN(L"ACTION"));
				const tree<ASTN>& instr = $6;
				act.add_child(instr);

				tree<ASTN> trans(ASTN(L"TRANSITION"));

				t.add_child(funcs);
				t.add_child(act);
				t.add_child(trans);
				$$ = t;
			}
		| STATE ID OBRACER function_list ACTION block_instructions TRANSITION OBRACER transition_list CBRACER CBRACER
			{
				tree<ASTN> t(ASTN(L"STATE", wstring($2.begin(), $2.end())));
				
				const tree<ASTN>& funcs = $4;

				tree<ASTN> act(ASTN(L"ACTION"));
				const tree<ASTN>& instr = $6;
				act.add_child(instr);

				tree<ASTN> trans = $9;

				t.add_child(funcs);
				t.add_child(act);
				t.add_child(trans);
				$$ = t;
			}
		;

// List of transitions
transition_list 	: TO ID IF expr
						{
							tree<ASTN> t(ASTN(L"TRANSITION"));
							tree<ASTN> trans(ASTN(L"TO"));
							tree<ASTN> id(ASTN(L"ID", wstring($2.begin(), $2.end())));
							const tree<ASTN>& cond = $4;

							trans.add_child(id);
							trans.add_child(cond);
							t.add_child(trans);
							$$ = t;
						}
					| transition_list TO ID IF expr
						{
							tree<ASTN> t = $1;
							tree<ASTN> trans(ASTN(L"TO"));
							tree<ASTN> id(ASTN(L"ID", wstring($3.begin(), $3.end())));
							const tree<ASTN>& cond = $5;

							trans.add_child(id);
							trans.add_child(cond);
							t.add_child(trans);
							$$ = t;
						}
					;

// A list of instructions, all of them gouped in a subtree
block_instructions	: OBRACER CBRACER
						{
							$$ = tree<ASTN> (ASTN(L"BODY"));
						}
					| OBRACER instruction_list CBRACER
						{
							$$ = $2;
						}
					;

// The list of instructions
instruction_list	: instruction SEMICOLON
						{
							tree<ASTN> body(ASTN(L"BODY"));
							const tree<ASTN>& instruction = $1;

							body.add_child(instruction);
							$$ = body;
						}
					| instruction_list instruction SEMICOLON
						{
							tree<ASTN>& body = $1;
							const tree<ASTN>& instruction = $2;

							body.add_child(instruction);
							$$ = body;
						}
					;

// The different types of instructions
instruction
		:	assign 			// Assignment
		|	ite_stmt		// if-then-else
		|	while_stmt		// while statement
		|	for_stmt		// for statement
		|	dowhile_stmt	// do while statement
		|	funcall			// Call to a procedure (no result produced)
		|	return_stmt		// Return statement
		;

// Assignment
assign	: ID ASSIGMENT expr
			{
				tree<ASTN> instr(ASTN(L"ASSIGMENT"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));
				const tree<ASTN>& body = $3;

				instr.add_child(id);
				instr.add_child(body);
				$$ = instr;
			}
		;

// if-then-else (else if & else are optionals)
ite_stmt	: IF OPARENTHESIS expr CPARENTHESIS block_instructions
				{
					tree<ASTN> ite(ASTN(L"IF"));
					const tree<ASTN>& condition = $3;
					const tree<ASTN>& body = $5;

					ite.add_child(condition);
					ite.add_child(body);
					$$ = ite;
				}
			| IF OPARENTHESIS expr CPARENTHESIS block_instructions else
				{
					tree<ASTN> ite(ASTN(L"IF ELSE"));
					const tree<ASTN>& condition = $3;
					const tree<ASTN>& body = $5;
					const tree<ASTN>& else_tree = $6;

					ite.add_child(condition);
					ite.add_child(body);
					ite.add_child(else_tree);
					$$ = ite;
				}
			| IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if
				{
					tree<ASTN> ite(ASTN(L"IF ELSE IF"));
					const tree<ASTN>& condition = $3;
					const tree<ASTN>& body = $5;
					const tree<ASTN>& elseif = $6;

					ite.add_child(condition);
					ite.add_child(body);
					ite.add_child(elseif);
					$$ = ite;
				}
			| IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if else
				{
					tree<ASTN> ite(ASTN(L"IF ELSE IF ELSE"));
					const tree<ASTN>& condition = $3;
					const tree<ASTN>& body = $5;
					const tree<ASTN>& elseif = $6;
					const tree<ASTN>& else_tree = $7;

					ite.add_child(condition);
					ite.add_child(body);
					ite.add_child(elseif);
					ite.add_child(else_tree);
					$$ = ite;
				}
			;

// else-if
else_if	: ELSE IF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ASTN> elif(ASTN(L"ELSE IF"));
				const tree<ASTN>& condition = $4;
				const tree<ASTN>& body = $6;

				elif.add_child(condition);
				elif.add_child(body);
				$$ = elif;
			}
		| else_if ELSE IF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ASTN>& elif = $1;
				const tree<ASTN>& condition = $5;
				const tree<ASTN>& body = $7;

				elif.add_child(condition);
				elif.add_child(body);
				$$ = elif;
			}
		;

// else
else 	: ELSE block_instructions
			{
				tree<ASTN> t(ASTN(L"ELSE"));
				const tree<ASTN>& body = $2;

				t.add_child(body);
				$$ = t;
			}
		;

// while statement
while_stmt	: WHILE OPARENTHESIS expr CPARENTHESIS block_instructions
				{
					tree<ASTN> t(ASTN(L"WHILE"));
					const tree<ASTN>& condition = $3;
					const tree<ASTN>& body = $5;

					t.add_child(condition);
					t.add_child(body);
					$$ = t;
				}
			;

// for statement
for_stmt	: FOR OPARENTHESIS expr SEMICOLON expr SEMICOLON expr CPARENTHESIS block_instructions
				{
					tree<ASTN> t(ASTN(L"FOR"));
					const tree<ASTN>& init = $3;
					const tree<ASTN>& condition = $5;
					const tree<ASTN>& final = $7;
					const tree<ASTN>& body = $9;

					t.add_child(init);
					t.add_child(condition);
					t.add_child(final);
					t.add_child(body);
					$$ = t;
				}
			;

// dowhile statement
dowhile_stmt	: DO block_instructions WHILE OPARENTHESIS expr CPARENTHESIS
				{
					tree<ASTN> dowhile(ASTN(L"DO WHILE"));
					const tree<ASTN>& body = $2;
					const tree<ASTN>& condition = $5;

					dowhile.add_child(body);
					dowhile.add_child(condition);
					$$ = dowhile;
				}

// Return statement with an expression
return_stmt	: RETURN
				{
					$$ = tree<ASTN> (ASTN(L"RETURN"));
				}
			| RETURN expr
				{
					tree<ASTN> t(ASTN(L"RETURN"));
					const tree<ASTN>& expr = $2;

					t.add_child(expr);
					$$ = t;
				}
			;

// Simple expression
expr: 
		BOOL
			{
				bool b = $1 == "true" || $1 == "TRUE";
				$$ = tree<ASTN>(ASTN(L"TOKEN BOOL", b));
			}
	|
		INT
			{
				$$ = tree<ASTN> (ASTN(L"TOKEN INT", $1));
			}
	|
		DOUBLE
			{
				$$ = tree<ASTN> (ASTN(L"TOKEN DOUBLE", $1));
			}
	|
		STRING
			{
				$$ = tree<ASTN> (ASTN(L"TOKEN STRING", wstring($1.begin(), $1.end())));
			}
	|
		ID
			{
				$$ = tree<ASTN> (ASTN(L"TOKEN ID", wstring($1.begin(), $1.end())));
			}
	|
		OBRACER CBRACER
			{
				$$ = tree<ASTN> (ASTN(L"TOKEN OBJECT"));
			}
	|
		OBRACER object_list CBRACER
			{
				tree<ASTN> n(ASTN(L"TOKEN OBJECT"));
				const tree<ASTN>& list = $2;

				n.add_child(list);
				$$ = n;
			}
	|
		OBRACKET CBRACKET
			{
				$$ = tree<ASTN> (ASTN(L"TOKEN ARRAY"));
			}
	|
		OBRACKET expr_list CBRACKET
			{
				tree<ASTN> n(ASTN(L"TOKEN ARRAY"));
				const tree<ASTN>& list = $2;

				n.add_child(list);
				$$ = n;
			}
	|
		funcall
			{
				$$ = $1;
			}
	|
		OPARENTHESIS expr CPARENTHESIS
			{
				$$ = $2;
			}
	|
		NOT expr
			{
				tree<ASTN> n(ASTN(L"NOT"));
				const tree<ASTN>& expr = $2;

				n.add_child(expr);
				$$ = n;
			}
	|
		MINUS expr %prec NEG
			{
				tree<ASTN> n(ASTN(L"NEG"));
				const tree<ASTN>& expr = $2;

				n.add_child(expr);
				$$ = n;
			}
	|
		PLUS expr %prec POS
			{
				tree<ASTN> n(ASTN(L"POS"));
				const tree<ASTN>& expr = $2;

				n.add_child(expr);
				$$ = n;
			}
	|
		expr PLUS expr
			{
				tree<ASTN> n(ASTN(L"PLUS"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr MINUS expr
			{
				tree<ASTN> n(ASTN(L"MINUS"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr MULT expr
			{
				tree<ASTN> n(ASTN(L"MULT"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr DIV expr
			{
				tree<ASTN> n(ASTN(L"DIV"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr MOD expr
			{
				tree<ASTN> n(ASTN(L"MOD"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr POW expr
			{
				tree<ASTN> n(ASTN(L"POW"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr EQUAL expr
			{
				tree<ASTN> n(ASTN(L"EQUAL"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr NOT_EQUAL expr
			{
				tree<ASTN> n(ASTN(L"NOT_EQUAL"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr LT expr
			{
				tree<ASTN> n(ASTN(L"LT"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr LE expr
			{
				tree<ASTN> n(ASTN(L"LE"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr GT expr
			{
				tree<ASTN> n(ASTN(L"GT"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr GE expr
			{
				tree<ASTN> n(ASTN(L"GE"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr AND expr
			{
				tree<ASTN> n(ASTN(L"AND"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		expr OR expr
			{
				tree<ASTN> n(ASTN(L"OR"));
				const tree<ASTN>& expr1 = $1;
				const tree<ASTN>& expr2 = $3;

				n.add_child(expr1);
				n.add_child(expr2);
				$$ = n;
			}
	|
		ID DOT ID
			{
				tree<ASTN> n(ASTN(L"OBJECT ACCES"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));
				tree<ASTN> pos(ASTN(L"POSITION", wstring($3.begin(), $3.end())));

				n.add_child(id);
				n.add_child(pos);
				$$ = n;
			}
	|
		ID OBRACKET INT CBRACKET
			{
				tree<ASTN> n(ASTN(L"ARRAY ACCES"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));
				tree<ASTN> pos(ASTN(L"POSITION", $3));

				n.add_child(id);
				n.add_child(pos);
				$$ = n;
			}
	|
		ID DOT ADD OPARENTHESIS expr COMMA INT CPARENTHESIS
			{
				tree<ASTN> n(ASTN(L"ARRAY ADD"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));
				const tree<ASTN>& element = $5;
				tree<ASTN> pos(ASTN(L"POSITION", $7));

				n.add_child(id);
				n.add_child(element);
				n.add_child(pos);
				$$ = n;
			}
	|
		ID DOT REMOVE OPARENTHESIS INT CPARENTHESIS
			{
				tree<ASTN> n(ASTN(L"ARRAY REMOVE"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));
				tree<ASTN> pos(ASTN(L"POSITION", $5));

				n.add_child(id);
				n.add_child(pos);
				$$ = n;
			}
	|
		ID DOT INDEXOF OPARENTHESIS expr CPARENTHESIS
			{
				tree<ASTN> n(ASTN(L"ARRAY INDEXOF"));
				tree<ASTN> id(ASTN(L"ID", wstring($1.begin(), $1.end())));
				const tree<ASTN>& element = $5;

				n.add_child(id);
				n.add_child(element);
				$$ = n;
			}
	;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall : ID OPARENTHESIS CPARENTHESIS
			{
				$$ = tree<ASTN> (ASTN(L"FUNCALL", wstring($1.begin(), $1.end())));;
			}
		| ID OPARENTHESIS expr_list CPARENTHESIS
			{
				tree<ASTN> funcall(ASTN(L"FUNCALL", wstring($1.begin(), $1.end())));
				const tree<ASTN>& list = $3;

				funcall.add_child(list);
				$$ = funcall;
			}
		;

// A list of expressions separated by commas
expr_list	: expr
				{
					tree<ASTN> list(ASTN(L"EXPR LIST"));
					const tree<ASTN>& expr = $1;

					list.add_child(expr);
					$$ = list;
				}
			| expr_list COMMA expr
				{
					tree<ASTN>& list = $1;
					const tree<ASTN>& expr = $3;

					list.add_child(expr);
					$$ = list;
				}
			;

// A list of declarations of the items of a dictionary
object_list 	: ID COLON expr
					{
						tree<ASTN> t(ASTN(L"OBJECT LIST"));
						tree<ASTN> obj(ASTN(L"ID", wstring($1.begin(), $1.end())));
						const tree<ASTN>& body = $3;

						obj.add_child(body);
						t.add_child(obj);
						$$ = t;
					}
				| object_list ID COLON expr
					{
						tree<ASTN>& t = $1;
						tree<ASTN> obj(ASTN(L"ID", wstring($2.begin(), $2.end())));
						const tree<ASTN>& body = $4;

						obj.add_child(body);
						t.add_child(obj);
						$$ = t;
					}
				;


%%


// Bison expects us to provide implementation - otherwise linker complains
void ATN::Parser::error(const location &loc , const std::string &message) {
    cout << "Error: " << message << endl << "Error location: " << driver.location() << endl << "Row: " << driver.row() << "  Column: " << driver.column() << endl;
}