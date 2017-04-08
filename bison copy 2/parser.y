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
    #include "astn.h"
    #include "atnn.h"
    #include "data.h"

    using namespace std;
    using namespace freeling;

    namespace ATN {
        class Scanner;
        class Atn;
    }
}

// This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// This function is called only inside Bison, so we make it static to limit
// symbol visibility for the linker to avoid potential linking conflicts.
%code top
{
	#include <iostream>
    #include <locale>
	#include <codecvt>
    #include "scanner.h"
    #include "parser.hpp"
    #include "atn.h"
    #include "location.hh"
    
    using namespace ATN;

    // yylex() arguments are defined in parser.y
    static Parser::symbol_type yylex(Scanner &scanner, Atn &driver) {
        return scanner.get_next_token();
    }

    wstring_convert< codecvt_utf8_utf16<wchar_t> > converter;
}


%lex-param { ATN::Scanner &scanner }
%lex-param { ATN::Atn &driver }
%parse-param { ATN::Scanner &scanner }
%parse-param { ATN::Atn &driver }
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
%token ENDL "end of line";
%token NOT_DEFINED "not defined";

%token PRINT "print";
%token PRINTSEPARATOR "<<";
%token IF "if";
%token ELSE "else";
%token ELSEIF "else if";
%token WHILE "while";
%token DO "do";
%token FOR "for";
%token FUNC "function";
%token RETURN "return";
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
%token <std::string> LOCALFUNCTION "local function";

%right 	ASSIGMENT
%left	OR
%left	AND
%left	EQUAL NOT_EQUAL LT LE GT GE
%left	PLUS MINUS DPLUS DMINUS
%left	MULT DIV MOD
%left	NEG POS NOT
%right	POW

%type < freeling::tree<ATN::ASTN*>* > mainElement func param_list param block_instructions ids init;
%type < freeling::tree<ATN::ASTN*>* > id_list state transition_list funcall element_to_print print_list;
%type < freeling::tree<ATN::ASTN*>* > instruction_list instruction assign ite_stmt else_if else object_list;
%type < freeling::tree<ATN::ASTN*>* > while_stmt for_stmt incremental dowhile_stmt return_stmt print_stmt expr;
%type < freeling::tree<ATN::ASTN*>* > atom arithmetical_expr boolean_expr expr_list local_functions double_arithmetic;
%type < ATN::ATNN* > atn;
%type < std::vector<std::wstring> > initials finals;
%type < std::map<std::wstring, freeling::tree<ATN::ASTN*>*> > states;
%type < std::map<std::wstring, ATN::Data* > > global;

%start program


%%


// A program is a list of main elements (ATN's, states, functions, global variables)
program 	: { driver.clear(); }
			| program mainElement
				{
					/* TODO eliminar todo esto
					tree<ASTN*>* t = $2;
					tree<ASTN*>::const_iterator it = t->begin();
					
					if (it->astn != nullptr) {
						wstring token = (it->astn)->getToken();
						if (token == L"FUNCTION") {
							driver.addMainElement((it->astn)->getValueWstring(), t);
						}
						else { // token == L"GLOBAL"
							for (int i = 0; i < t->num_children(); ++i) {
								tree<ASTN*>* tr = new tree<ASTN*>(it.nth_child_ref(i));
								driver.addMainElement(((tr->begin())->astn)->getValueWstring(), new Data());
							}
						}
					}
					else { // it->atn != nullptr
						driver.addMainElement((it->atn)->getName(), t);
					}
					*/
				}
		;

// A main element is an element as the type of: ATN, Function, Global variables
mainElement	: global SEMICOLON 
				{
					driver.m_global = $1;
				}
			| func
	   			{
	   				tree<ASTN*>* t = $1;
	   				tree<ASTN*>::const_iterator it = t->begin();
	   				driver.m_func[(*it)->getValueWstring()] = t;
	   			}
			| atn
	 			{
	 				ATNN* t = $1;
	   				driver.m_atn[t->getName()] = t;
	 			}
			;

// A global variable
global 	: GLOBAL ID
			{
				map<wstring, Data* > m;
				m[converter.from_bytes($2)] = new Data();
				$$ = m;

				/* TODO
				ASTN* astn1(new ASTN(L"GLOBAL"));
				tree<ASTN*>* t = new tree<ASTN*>(astn1);
				ASTN* astn2(new ASTN(L"GLOBAL ID", converter.from_bytes($2)));
				tree<ASTN*>* id = new tree<ASTN*>(astn2);

				t->add_child(*id);
				$$ = t;
				*/
			}
		| global COMMA ID
			{
				map<wstring, Data* > m = $1;
				m[converter.from_bytes($3)] = new Data();
				$$ = m;

				/* TODO
				tree<ASTN*>* t = $1;
				ASTN* astn(new ASTN(L"GLOBAL ID", converter.from_bytes($3)));
				tree<ASTN*>* id = new tree<ASTN*>(astn);

				t->add_child(*id);
				$$ = t;
				*/
			}
		;

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID OPARENTHESIS param_list CPARENTHESIS block_instructions
			{
				tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"FUNCTION", converter.from_bytes($2)));
				const tree<ASTN*>* params = $4;
				const tree<ASTN*>* body = $6;

				t->add_child(*params);
				t->add_child(*body);
				$$ = t;
			}
		;

// The list of parameters grouped in a subtree (it can be empty)
param_list	: // nothing
				{
					$$ = new tree<ASTN*>(new ASTN(L"PARAM LIST"));
				}
			| param
				{
					tree<ASTN*>* params = new tree<ASTN*>(new ASTN(L"PARAM LIST"));
					tree<ASTN*>* p = $1;

					params->add_child(*p);
					$$ = params;
				}
			| param_list COMMA param
				{
					tree<ASTN*>* params = $1;
					tree<ASTN*>* p = $3;

					params->add_child(*p);
					$$ = params;
				}
			;

// Parameters with & as prefix are passed by reference
// Only one ASTN* with the name of the parameter is created
param 	: ANDPERSAND ID
			{
				$$ = new tree<ASTN*>(new ASTN(L"ANDPERSAND", converter.from_bytes($2)));
			}
		| ID
			{
				$$ = new tree<ASTN*>(new ASTN(L"VALUE", converter.from_bytes($1)));
			}
		;

// ATN
atn 	: ATN ID OBRACER initials finals states CBRACER
			{
				ATNN* atnn = new ATNN(converter.from_bytes($2));

				vector<wstring> init = $4;
				vector<wstring> fin = $5;
				map<wstring, tree<ASTN*>*> sts = $6;
				
				atnn->setInitials(init);
				atnn->setFinals(fin);
				atnn->setStates(sts);
				$$ = atnn;
			}
		| ATN ID OBRACER initials states finals CBRACER
			{
				ATNN* atnn = new ATNN(converter.from_bytes($2));

				vector<wstring> init = $4;
				vector<wstring> fin = $6;
				map<wstring, tree<ASTN*>*> sts = $5;
				
				atnn->setInitials(init);
				atnn->setFinals(fin);
				atnn->setStates(sts);
				$$ = atnn;
			}
		| ATN ID OBRACER finals initials states CBRACER
			{
				ATNN* atnn = new ATNN(converter.from_bytes($2));

				vector<wstring> init = $5;
				vector<wstring> fin = $4;
				map<wstring, tree<ASTN*>*> sts = $6;
				
				atnn->setInitials(init);
				atnn->setFinals(fin);
				atnn->setStates(sts);
				$$ = atnn;
			}
		| ATN ID OBRACER finals states initials CBRACER
			{
				ATNN* atnn = new ATNN(converter.from_bytes($2));

				vector<wstring> init = $6;
				vector<wstring> fin = $4;
				map<wstring, tree<ASTN*>*> sts = $5;
				
				atnn->setInitials(init);
				atnn->setFinals(fin);
				atnn->setStates(sts);
				$$ = atnn;
			}
		| ATN ID OBRACER states initials finals CBRACER
			{
				ATNN* atnn = new ATNN(converter.from_bytes($2));

				vector<wstring> init = $5;
				vector<wstring> fin = $6;
				map<wstring, tree<ASTN*>*> sts = $4;
				
				atnn->setInitials(init);
				atnn->setFinals(fin);
				atnn->setStates(sts);
				$$ = atnn;
			}
		| ATN ID OBRACER states finals initials CBRACER
			{
				ATNN* atnn = new ATNN(converter.from_bytes($2));

				vector<wstring> init = $6;
				vector<wstring> fin = $5;
				map<wstring, tree<ASTN*>*> sts = $4;

				atnn->setInitials(init);
				atnn->setFinals(fin);
				atnn->setStates(sts);
				$$ = atnn;
			}
		;

// The initials states of an ATN
initials 	: INITIAL COLON id_list SEMICOLON
				{
					vector<wstring> v;
					tree<ASTN*>* t = $3;
					tree<ASTN*>::const_iterator it = t->begin();
					for (int i = 0; i < t->num_children(); ++i) {
						tree<ASTN*>* tr = new tree<ASTN*>(it.nth_child_ref(i));
						wstring id = (*(tr->begin()))->getValueWstring();
						v.push_back(id);
					}
					$$ = v;
				}
			;

// The finals states of an ATN
finals 	: FINAL COLON id_list SEMICOLON
				{
					vector<wstring> v;
					tree<ASTN*>* t = $3;
					tree<ASTN*>::const_iterator it = t->begin();
					for (int i = 0; i < t->num_children(); ++i) {
						tree<ASTN*>* tr = new tree<ASTN*>(it.nth_child_ref(i));
						wstring id = (*(tr->begin()))->getValueWstring();
						v.push_back(id);
					}
					$$ = v;
				}
			;

// List of ids
id_list	: ID
			{
				tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"ID LIST"));
				tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));

				t->add_child(*id);
				$$ = t;
			}
		| id_list COMMA ID
			{
				tree<ASTN*>* t = $1;
				tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($3)));

				t->add_child(*id);
				$$ = t;
			}
		;

// States of the ATN
states 	: state
			{
				map<wstring, tree<ASTN*>*> m;
				tree<ASTN*>* t = $1;
				m[(*(t->begin()))->getValueWstring()] = t;
				$$ = m;
			}
		| states state
			{
				map<wstring, tree<ASTN*>*> m = $1;
				tree<ASTN*>* t = $2;
				m[(*(t->begin()))->getValueWstring()] = t;
				$$ = m;
			}
		;

// Single state
state 	: STATE ID OBRACER ACTION block_instructions TRANSITION OBRACER transition_list CBRACER CBRACER
			{
				tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"STATE", converter.from_bytes($2)));
				
				tree<ASTN*>* act = new tree<ASTN*>(new ASTN(L"ACTION"));
				tree<ASTN*>* instr = $5;
				act->add_child(*instr);

				tree<ASTN*>* trans = $8;

				t->add_child(*act);
				t->add_child(*trans);
				$$ = t;
			}
		| STATE ID OBRACER TRANSITION OBRACER transition_list CBRACER ACTION block_instructions CBRACER
			{
				tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"STATE", converter.from_bytes($2)));
				
				tree<ASTN*>* act = new tree<ASTN*>(new ASTN(L"ACTION"));
				tree<ASTN*>* instr = $9;
				act->add_child(*instr);

				tree<ASTN*>* trans = $6;

				t->add_child(*act);
				t->add_child(*trans);
				$$ = t;
			}
		;

// List of transitions
transition_list 	: // nothing
						{
							$$ = new tree<ASTN*>(new ASTN(L"TRANSITION"));
						}
					| TO ID IF OPARENTHESIS expr CPARENTHESIS SEMICOLON
						{
							tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"TRANSITION"));
							tree<ASTN*>* trans = new tree<ASTN*>(new ASTN(L"TO"));
							tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($2)));
							tree<ASTN*>* cond = $5;

							trans->add_child(*id);
							trans->add_child(*cond);
							t->add_child(*trans);
							$$ = t;
						}
					| transition_list TO ID IF OPARENTHESIS expr CPARENTHESIS SEMICOLON
						{
							tree<ASTN*>* t = $1;
							tree<ASTN*>* trans = new tree<ASTN*>(new ASTN(L"TO"));
							tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($3)));
							tree<ASTN*>* cond = $6;

							trans->add_child(*id);
							trans->add_child(*cond);
							t->add_child(*trans);
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
							$$ = new tree<ASTN*>(new ASTN(L"BODY"));
						}
					| instruction
						{
							tree<ASTN*>* body = new tree<ASTN*>(new ASTN(L"BODY"));
							tree<ASTN*>* instruction = $1;

							body->add_child(*instruction);
							$$ = body;
						}
					| instruction_list instruction
						{
							tree<ASTN*>* body = $1;
							tree<ASTN*>* instruction = $2;

							body->add_child(*instruction);
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
assign	: ids ASSIGMENT expr
			{
				tree<ASTN*>* instr = new tree<ASTN*>(new ASTN(L"ASSIGMENT"));
				tree<ASTN*>* id = $1;
				tree<ASTN*>* body = $3;

				instr->add_child(*id);
				instr->add_child(*body);
				$$ = instr;
			}
		;

// Valid id
ids 	: ID
			{
				$$ = new tree<ASTN*>(new ASTN(L"TOKEN ID", converter.from_bytes($1)));
			}
		| ID DOT ID
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"OBJECT ACCES"));
				tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ASTN*>* pos = new tree<ASTN*>(new ASTN(L"POSITION", converter.from_bytes($3)));

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		| ID OBRACKET expr CBRACKET
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"ARRAY ACCES"));
				tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ASTN*>* pos = $3;

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		;

// if-then-else (else if & else are optionals)
ite_stmt	: IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if else
				{
					tree<ASTN*>* ite = new tree<ASTN*>(new ASTN(L"IF ELSE IF ELSE"));
					tree<ASTN*>* condition = $3;
					tree<ASTN*>* body = $5;
					tree<ASTN*>* elseif = $6;
					tree<ASTN*>* else_tree = $7;

					ite->add_child(*condition);
					ite->add_child(*body);
					ite->add_child(*elseif);
					ite->add_child(*else_tree);
					$$ = ite;
				}
			;

// else-if
else_if	: // nothing
			{
				$$ = new tree<ASTN*>(new ASTN(L"ELSE IF"));
			}
		| ELSEIF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ASTN*>* elif = new tree<ASTN*>(new ASTN(L"ELSE IF"));
				tree<ASTN*>* condition = $3;
				tree<ASTN*>* body = $5;

				elif->add_child(*condition);
				elif->add_child(*body);
				$$ = elif;
			}
		| else_if ELSEIF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ASTN*>* elif = $1;
				tree<ASTN*>* condition = $4;
				tree<ASTN*>* body = $6;

				elif->add_child(*condition);
				elif->add_child(*body);
				$$ = elif;
			}
		;

// else
else 	: // nothing
			{
				$$ = new tree<ASTN*>(new ASTN(L"ELSE"));
			}
		| ELSE block_instructions
			{
				tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"ELSE"));
				tree<ASTN*>* body = $2;

				t->add_child(*body);
				$$ = t;
			}
		;

// while statement
while_stmt	: WHILE OPARENTHESIS expr CPARENTHESIS block_instructions
				{
					tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"WHILE"));
					tree<ASTN*>* condition = $3;
					tree<ASTN*>* body = $5;

					t->add_child(*condition);
					t->add_child(*body);
					$$ = t;
				}
			;

// for statement
for_stmt	: FOR OPARENTHESIS init SEMICOLON expr SEMICOLON incremental CPARENTHESIS block_instructions
				{
					tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"FOR"));
					tree<ASTN*>* init = $3;
					tree<ASTN*>* condition = $5;
					tree<ASTN*>* final = $7;
					tree<ASTN*>* body = $9;

					t->add_child(*init);
					t->add_child(*condition);
					t->add_child(*final);
					t->add_child(*body);
					$$ = t;
				}
			;

// assigment or nothing
init 	: // nothing
			{ $$ = new tree<ASTN*>(new ASTN(L"ASSIGMENT")); }
		| assign
			{ $$ = $1; }
		;

// assigment or expr
incremental 	: // nothing
					{ $$ = new tree<ASTN*>(new ASTN(L"INCREMENTAL")); }
				| assign
					{ $$ = $1; }
				| double_arithmetic
					{ $$ = $1; }
				;

// dowhile statement
dowhile_stmt	: DO block_instructions WHILE OPARENTHESIS expr CPARENTHESIS
				{
					tree<ASTN*>* dowhile = new tree<ASTN*>(new ASTN(L"DO WHILE"));
					tree<ASTN*>* body = $2;
					tree<ASTN*>* condition = $5;

					dowhile->add_child(*body);
					dowhile->add_child(*condition);
					$$ = dowhile;
				}

// Return statement with an expression
return_stmt	: RETURN
				{
					$$ = new tree<ASTN*>(new ASTN(L"RETURN"));
				}
			| RETURN expr
				{
					tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"RETURN"));
					tree<ASTN*>* expr = $2;

					t->add_child(*expr);
					$$ = t;
				}
			;

// Print statement
print_stmt 	: PRINT print_list
				{
					tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"PRINT"));
					tree<ASTN*>* e = $2;

					t->add_child(*e);
					$$ = t;
				}
			;

// list of exprsions to print
print_list 	: PRINTSEPARATOR element_to_print
				{
					tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"PRINT LIST"));
					tree<ASTN*>* e = $2;

					t->add_child(*e);
					$$ = t;
				}
			| print_list PRINTSEPARATOR element_to_print
				{
					tree<ASTN*>* t = $1;
					tree<ASTN*>* e = $3;

					t->add_child(*e);
					$$ = t;
				}
			;

element_to_print 	: expr
						{ $$ = $1; }
					| ENDL
						{ $$ = new tree<ASTN*>(new ASTN(L"ENDL")); }
					;

// Simple expression
expr : atom 					{ $$ = $1; }
	 | arithmetical_expr		{ $$ = $1; }
	 | boolean_expr				{ $$ = $1; }
	 | local_functions			{ $$ = $1; }
	 ;

// The simplest kind of expr
atom 	: BOOL
			{
				bool b = $1 == "true" || $1 == "TRUE";
				$$ = new tree<ASTN*>(new ASTN(L"TOKEN BOOL", b));
			}
		| INT
			{
				$$ = new tree<ASTN*>(new ASTN(L"TOKEN INT", $1));
			}
		| DOUBLE
			{
				$$ = new tree<ASTN*>(new ASTN(L"TOKEN DOUBLE", $1));
			}
		| STRING
			{
				string s = $1.substr(1, $1.length() - 2);
				$$ = new tree<ASTN*>(new ASTN(L"TOKEN STRING", converter.from_bytes(s)));
			}
		| ID
			{
				$$ = new tree<ASTN*>(new ASTN(L"TOKEN ID", converter.from_bytes($1)));
			}
		| ID DOT ID
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"OBJECT ACCES"));
				tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ASTN*>* pos = new tree<ASTN*>(new ASTN(L"POSITION", converter.from_bytes($3)));

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		| ID OBRACKET expr CBRACKET
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"ARRAY ACCES"));
				tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ASTN*>* pos = $3;

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		| OBRACER object_list CBRACER
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"TOKEN OBJECT"));
				tree<ASTN*>* list = $2;

				n->add_child(*list);
				$$ = n;
			}
		| OBRACKET expr_list CBRACKET
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"TOKEN ARRAY"));
				tree<ASTN*>* list = $2;

				n->add_child(*list);
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
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"NOT"));
				tree<ASTN*>* expr = $2;

				n->add_child(*expr);
				$$ = n;
			}
		| MINUS expr %prec NEG
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"NEG"));
				tree<ASTN*>* expr = $2;

				n->add_child(*expr);
				$$ = n;
			}
		| PLUS expr %prec POS
			{
				tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"POS"));
				tree<ASTN*>* expr = $2;

				n->add_child(*expr);
				$$ = n;
			}
		| double_arithmetic
			{
				$$ = $1;
			}
		;


// Arithmetical expr
arithmetical_expr 	: expr PLUS expr
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"PLUS"));
							tree<ASTN*>* expr1 = $1;
							tree<ASTN*>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr MINUS expr
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"MINUS"));
							tree<ASTN*>* expr1 = $1;
							tree<ASTN*>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr MULT expr
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"MULT"));
							tree<ASTN*>* expr1 = $1;
							tree<ASTN*>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr DIV expr
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"DIV"));
							tree<ASTN*>* expr1 = $1;
							tree<ASTN*>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr MOD expr
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"MOD"));
							tree<ASTN*>* expr1 = $1;
							tree<ASTN*>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					;

// Boolean expr
boolean_expr 	: expr EQUAL expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"EQUAL"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr NOT_EQUAL expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"NOT_EQUAL"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr LT expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"LT"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr LE expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"LE"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr GT expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"GT"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr GE expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"GE"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr AND expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"AND"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr OR expr
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"OR"));
						tree<ASTN*>* expr1 = $1;
						tree<ASTN*>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				;

// local functions
local_functions : ID DOT LOCALFUNCTION OPARENTHESIS expr_list CPARENTHESIS
					{
						tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"LOCAL FUNCTION", converter.from_bytes($3)));
						tree<ASTN*>* id = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));
						tree<ASTN*>* list = $5;

						n->add_child(*id);
						n->add_child(*list);
						$$ = n;
					}
				;

// double plus or double minus
double_arithmetic 	: DPLUS ids
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"DPLUS"));
							tree<ASTN*>* e = $2;

							n->add_child(*e);
							$$ = n;
						}
					|  ids DPLUS
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"DPLUSR"));
							tree<ASTN*>* e = $1;

							n->add_child(*e);
							$$ = n;
						}
					| DMINUS ids
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"DMINUS"));
							tree<ASTN*>* e = $2;

							n->add_child(*e);
							$$ = n;
						}
					| ids DMINUS
						{
							tree<ASTN*>* n = new tree<ASTN*>(new ASTN(L"DMINUSR"));
							tree<ASTN*>* e = $1;

							n->add_child(*e);
							$$ = n;
						}
					;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall : ID OPARENTHESIS expr_list CPARENTHESIS
			{
				tree<ASTN*>* funcall = new tree<ASTN*>(new ASTN(L"FUNCALL", converter.from_bytes($1)));
				tree<ASTN*>* list = $3;

				funcall->add_child(*list);
				$$ = funcall;
			}
		;

// A list of expressions separated by commas
expr_list	: // nothing
				{
					$$ = new tree<ASTN*>(new ASTN(L"EXPR LIST"));
				}
			| expr
				{
					tree<ASTN*>* list = new tree<ASTN*>(new ASTN(L"EXPR LIST"));
					tree<ASTN*>* expr = $1;

					list->add_child(*expr);
					$$ = list;
				}
			| expr_list COMMA expr
				{
					tree<ASTN*>* list = $1;
					tree<ASTN*>* expr = $3;

					list->add_child(*expr);
					$$ = list;
				}
			;

// A list of declarations of the items of a dictionary
object_list 	: // nothing
					{
						$$ = new tree<ASTN*>(new ASTN(L"OBJECT LIST"));
					}
				| ID COLON expr
					{
						tree<ASTN*>* t = new tree<ASTN*>(new ASTN(L"OBJECT LIST"));
						tree<ASTN*>* obj = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($1)));
						tree<ASTN*>* body = $3;

						obj->add_child(*body);
						t->add_child(*obj);
						$$ = t;
					}
				| object_list COMMA ID COLON expr
					{
						tree<ASTN*>* t = $1;
						tree<ASTN*>* obj = new tree<ASTN*>(new ASTN(L"ID", converter.from_bytes($3)));
						tree<ASTN*>* body = $5;

						obj->add_child(*body);
						t->add_child(*obj);
						$$ = t;
					}
				;


%%


// Bison expects us to provide implementation - otherwise linker complains
void ATN::Parser::error(const location &loc , const std::string &message) {
    cout << "Error: " << message << endl << "Error location: " << driver.location() << endl << "Row: " << driver.row() << "  Column: " << driver.column() << endl;
}