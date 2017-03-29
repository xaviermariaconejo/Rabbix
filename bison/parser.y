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

%type < freeling::tree<ATN::ATNN::Node>* > mainElement global func param_list param block_instructions ids init;
%type < freeling::tree<ATN::ATNN::Node>* > atn id_list state transition_list funcall element_to_print print_list;
%type < freeling::tree<ATN::ATNN::Node>* > instruction_list instruction assign ite_stmt else_if else object_list;
%type < freeling::tree<ATN::ATNN::Node>* > while_stmt for_stmt incremental dowhile_stmt return_stmt print_stmt expr states;
%type < freeling::tree<ATN::ATNN::Node>* > atom arithmetical_expr boolean_expr expr_list local_functions double_arithmetic;
%type < std::map<std::wstring, freeling::tree<ATN::ATNN::Node>*> > finals;
%type < std::vector<std::wstring> > initials;

%start program


%%


// A program is a list of main elements (ATN's, states, functions, global variables)
program 	: { driver.clear(); }
			| program mainElement
				{
					tree<ATNN::Node>* t = $2;
					tree<ATNN::Node>::const_iterator it = t->begin();
					
					if (it->astn != nullptr) {
						wstring token = (it->astn)->getToken();
						if (token == L"FUNCTION") {
							driver.addMainElement((it->astn)->getValueWstring(), t);
						}
						else { // token == L"GLOBAL"
							for (int i = 0; i < t->num_children(); ++i) {
								tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
								driver.addMainElement(((tr->begin())->astn)->getValueWstring(), new Data());
							}
						}
					}
					else { // it->atn != nullptr
						driver.addMainElement((it->atn)->getName(), t);
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
				ATNN::Node astn1(new ASTN(L"GLOBAL"));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn1);
				ATNN::Node astn2(new ASTN(L"GLOBAL ID", converter.from_bytes($2)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);

				t->add_child(*id);
				$$ = t;
			}
		| global COMMA ID
			{
				tree<ATNN::Node>* t = $1;
				ATNN::Node astn(new ASTN(L"GLOBAL ID", converter.from_bytes($3)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn);

				t->add_child(*id);
				$$ = t;
			}
		;

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID OPARENTHESIS param_list CPARENTHESIS block_instructions
			{
				ATNN::Node astn(new ASTN(L"FUNCTION", converter.from_bytes($2)));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
				const tree<ATNN::Node>* params = $4;
				const tree<ATNN::Node>* body = $6;

				t->add_child(*params);
				t->add_child(*body);
				$$ = t;
			}
		;

// The list of parameters grouped in a subtree (it can be empty)
param_list	: // nothing
				{
					ATNN::Node astn(new ASTN(L"PARAM LIST"));
					$$ = new tree<ATNN::Node>(astn);
				}
			| param
				{
					ATNN::Node astn(new ASTN(L"PARAM LIST"));
					tree<ATNN::Node>* params = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* p = $1;

					params->add_child(*p);
					$$ = params;
				}
			| param_list COMMA param
				{
					tree<ATNN::Node>* params = $1;
					tree<ATNN::Node>* p = $3;

					params->add_child(*p);
					$$ = params;
				}
			;

// Parameters with & as prefix are passed by reference
// Only one ATNN::node with the name of the parameter is created
param 	: ANDPERSAND ID
			{
				ATNN::Node astn(new ASTN(L"ANDPERSAND", converter.from_bytes($2)));
				$$ = new tree<ATNN::Node>(astn);
			}
		| ID
			{
				ATNN::Node astn(new ASTN(L"VALUE", converter.from_bytes($1)));
				$$ = new tree<ATNN::Node>(astn);
			}
		;

// ATN
atn 	: ATN ID OBRACER initials finals states CBRACER
			{
				ATNN::Node atnn(new ATNN(converter.from_bytes($2)));

				vector<wstring> init = $4;
				map<wstring, tree<ATNN::Node>*> fin = $5;
				map<wstring, tree<ATNN::Node>*> sts;
				
				tree<ATNN::Node>* t = $6;
				tree<ATNN::Node>::const_iterator it = t->begin();
				for (int i = 0; i < t->num_children(); ++i) {
					tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
					wstring id = ((tr->begin())->astn)->getValueWstring();
					sts[id] = tr;
				}

				atnn.atn->setInitials(init);
				atnn.atn->setFinals(fin);
				atnn.atn->setStates(sts);
				$$ = new tree<ATNN::Node>(atnn);
			}
		| ATN ID OBRACER initials states finals CBRACER
			{
				ATNN::Node atnn(new ATNN(converter.from_bytes($2)));

				vector<wstring> init = $4;
				map<wstring, tree<ATNN::Node>*> fin = $6;
				map<wstring, tree<ATNN::Node>*> sts;
				
				tree<ATNN::Node>* t = $5;
				tree<ATNN::Node>::const_iterator it = t->begin();
				for (int i = 0; i < t->num_children(); ++i) {
					tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
					wstring id = ((tr->begin())->astn)->getValueWstring();
					sts[id] = tr;
				}

				atnn.atn->setInitials(init);
				atnn.atn->setFinals(fin);
				atnn.atn->setStates(sts);
				$$ = new tree<ATNN::Node>(atnn);
			}
		| ATN ID OBRACER finals initials states CBRACER
			{
				ATNN::Node atnn(new ATNN(converter.from_bytes($2)));

				vector<wstring> init = $5;
				map<wstring, tree<ATNN::Node>*> fin = $4;
				map<wstring, tree<ATNN::Node>*> sts;
				
				tree<ATNN::Node>* t = $6;
				tree<ATNN::Node>::const_iterator it = t->begin();
				for (int i = 0; i < t->num_children(); ++i) {
					tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
					wstring id = ((tr->begin())->astn)->getValueWstring();
					sts[id] = tr;
				}

				atnn.atn->setInitials(init);
				atnn.atn->setFinals(fin);
				atnn.atn->setStates(sts);
				$$ = new tree<ATNN::Node>(atnn);
			}
		| ATN ID OBRACER finals states initials CBRACER
			{
				ATNN::Node atnn(new ATNN(converter.from_bytes($2)));

				vector<wstring> init = $6;
				map<wstring, tree<ATNN::Node>*> fin = $4;
				map<wstring, tree<ATNN::Node>*> sts;
				
				tree<ATNN::Node>* t = $5;
				tree<ATNN::Node>::const_iterator it = t->begin();
				for (int i = 0; i < t->num_children(); ++i) {
					tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
					wstring id = ((tr->begin())->astn)->getValueWstring();
					sts[id] = tr;
				}

				atnn.atn->setInitials(init);
				atnn.atn->setFinals(fin);
				atnn.atn->setStates(sts);
				$$ = new tree<ATNN::Node>(atnn);
			}
		| ATN ID OBRACER states initials finals CBRACER
			{
				ATNN::Node atnn(new ATNN(converter.from_bytes($2)));

				vector<wstring> init = $5;
				map<wstring, tree<ATNN::Node>*> fin = $6;
				map<wstring, tree<ATNN::Node>*> sts;
				
				tree<ATNN::Node>* t = $4;
				tree<ATNN::Node>::const_iterator it = t->begin();
				for (int i = 0; i < t->num_children(); ++i) {
					tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
					wstring id = ((tr->begin())->astn)->getValueWstring();
					sts[id] = tr;
				}

				atnn.atn->setInitials(init);
				atnn.atn->setFinals(fin);
				atnn.atn->setStates(sts);
				$$ = new tree<ATNN::Node>(atnn);
			}
		| ATN ID OBRACER states finals initials CBRACER
			{
				ATNN::Node atnn(new ATNN(converter.from_bytes($2)));

				vector<wstring> init = $6;
				map<wstring, tree<ATNN::Node>*> fin = $5;
				map<wstring, tree<ATNN::Node>*> sts;

				tree<ATNN::Node>* t = $4;
				tree<ATNN::Node>::const_iterator it = t->begin();
				for (int i = 0; i < t->num_children(); ++i) {
					tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
					wstring id = ((tr->begin())->astn)->getValueWstring();
					sts[id] = tr;
				}

				atnn.atn->setInitials(init);
				atnn.atn->setFinals(fin);
				atnn.atn->setStates(sts);
				$$ = new tree<ATNN::Node>(atnn);
			}
		;

// The initials states of an ATN
initials 	: INITIAL COLON id_list SEMICOLON
				{
					vector<wstring> v;
					tree<ATNN::Node>* t = $3;
					tree<ATNN::Node>::const_iterator it = t->begin();
					for (int i = 0; i < t->num_children(); ++i) {
						tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
						wstring id = ((tr->begin())->astn)->getValueWstring();
						v.push_back(id);
					}
					$$ = v;
				}
			;

// The finals states of an ATN
finals 	: FINAL COLON id_list SEMICOLON
				{
					map<wstring, tree<ATNN::Node>*> m;
					tree<ATNN::Node>* t = $3;
					tree<ATNN::Node>::const_iterator it = t->begin();
					for (int i = 0; i < t->num_children(); ++i) {
						tree<ATNN::Node>* tr = new tree<ATNN::Node>(it.nth_child_ref(i));
						wstring id = ((tr->begin())->astn)->getValueWstring();
						m[id] = tr;
					}
					$$ = m;
				}
			;

// List of ids
id_list	: ID
			{
				ATNN::Node astn1(new ASTN(L"ID LIST"));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn1);
				ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);

				t->add_child(*id);
				$$ = t;
			}
		| id_list COMMA ID
			{
				tree<ATNN::Node>* t = $1;
				ATNN::Node astn(new ASTN(L"ID", converter.from_bytes($3)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn);

				t->add_child(*id);
				$$ = t;
			}
		;

// States of the ATN
states 	: state
			{
				ATNN::Node astn(new ASTN(L"STATES"));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
				t->add_child(*$1);
				$$ = t;
			}
		| states state
			{
				tree<ATNN::Node>* t = $1;
				t->add_child(*$2);
				$$ = $1;
			}
		;

// Single state
state 	: STATE ID OBRACER ACTION block_instructions TRANSITION OBRACER transition_list CBRACER CBRACER
			{
				ATNN::Node astn1(new ASTN(L"STATE", converter.from_bytes($2)));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn1);
				
				ATNN::Node astn2(new ASTN(L"ACTION"));
				tree<ATNN::Node>* act = new tree<ATNN::Node>(astn2);
				tree<ATNN::Node>* instr = $5;
				act->add_child(*instr);

				tree<ATNN::Node>* trans = $8;

				t->add_child(*act);
				t->add_child(*trans);
				$$ = t;
			}
		| STATE ID OBRACER TRANSITION OBRACER transition_list CBRACER ACTION block_instructions CBRACER
			{
				ATNN::Node astn1(new ASTN(L"STATE", converter.from_bytes($2)));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn1);
				
				ATNN::Node astn2(new ASTN(L"ACTION"));
				tree<ATNN::Node>* act = new tree<ATNN::Node>(astn2);
				tree<ATNN::Node>* instr = $9;
				act->add_child(*instr);

				tree<ATNN::Node>* trans = $6;

				t->add_child(*act);
				t->add_child(*trans);
				$$ = t;
			}
		;

// List of transitions
transition_list 	: // nothing
						{
							ATNN::Node astn(new ASTN(L"TRANSITION"));
							$$ = new tree<ATNN::Node>(astn);
						}
					| TO ID IF OPARENTHESIS expr CPARENTHESIS SEMICOLON
						{
							ATNN::Node astn1(new ASTN(L"TRANSITION"));
							tree<ATNN::Node>* t = new tree<ATNN::Node>(astn1);
							ATNN::Node astn2(new ASTN(L"TO"));
							tree<ATNN::Node>* trans = new tree<ATNN::Node>(astn2);
							ATNN::Node astn3(new ASTN(L"ID", converter.from_bytes($2)));
							tree<ATNN::Node>* id = new tree<ATNN::Node>(astn3);
							tree<ATNN::Node>* cond = $5;

							trans->add_child(*id);
							trans->add_child(*cond);
							t->add_child(*trans);
							$$ = t;
						}
					| transition_list TO ID IF OPARENTHESIS expr CPARENTHESIS SEMICOLON
						{
							tree<ATNN::Node>* t = $1;
							ATNN::Node astn1(new ASTN(L"TO"));
							tree<ATNN::Node>* trans = new tree<ATNN::Node>(astn1);
							ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($3)));
							tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);
							tree<ATNN::Node>* cond = $6;

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
							ATNN::Node astn(new ASTN(L"BODY"));
							$$ = new tree<ATNN::Node>(astn);
						}
					| instruction
						{
							ATNN::Node astn(new ASTN(L"BODY"));
							tree<ATNN::Node>* body = new tree<ATNN::Node>(astn);
							tree<ATNN::Node>* instruction = $1;

							body->add_child(*instruction);
							$$ = body;
						}
					| instruction_list instruction
						{
							tree<ATNN::Node>* body = $1;
							tree<ATNN::Node>* instruction = $2;

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
				ATNN::Node astn(new ASTN(L"ASSIGMENT"));
				tree<ATNN::Node>* instr = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* id = $1;
				tree<ATNN::Node>* body = $3;

				instr->add_child(*id);
				instr->add_child(*body);
				$$ = instr;
			}
		;

// Valid id
ids 	: ID
			{
				ATNN::Node astn(new ASTN(L"TOKEN ID", converter.from_bytes($1)));
				$$ = new tree<ATNN::Node>(astn);
			}
		| ID DOT ID
			{
				ATNN::Node astn1(new ASTN(L"OBJECT ACCES"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn1);
				ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);
				ATNN::Node astn3(new ASTN(L"POSITION", converter.from_bytes($3)));
				tree<ATNN::Node>* pos = new tree<ATNN::Node>(astn3);

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		| ID OBRACKET expr CBRACKET
			{
				ATNN::Node astn1(new ASTN(L"ARRAY ACCES"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn1);
				ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);
				tree<ATNN::Node>* pos = $3;

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		;

// if-then-else (else if & else are optionals)
ite_stmt	: IF OPARENTHESIS expr CPARENTHESIS block_instructions else_if else
				{
					ATNN::Node astn(new ASTN(L"IF ELSE IF ELSE"));
					tree<ATNN::Node>* ite = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* condition = $3;
					tree<ATNN::Node>* body = $5;
					tree<ATNN::Node>* elseif = $6;
					tree<ATNN::Node>* else_tree = $7;

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
				ATNN::Node astn(new ASTN(L"ELSE IF"));
				$$ = new tree<ATNN::Node>(astn);
			}
		| ELSEIF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				ATNN::Node astn(new ASTN(L"ELSE IF"));
				tree<ATNN::Node>* elif = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* condition = $3;
				tree<ATNN::Node>* body = $5;

				elif->add_child(*condition);
				elif->add_child(*body);
				$$ = elif;
			}
		| else_if ELSEIF OPARENTHESIS expr CPARENTHESIS block_instructions
			{
				tree<ATNN::Node>* elif = $1;
				tree<ATNN::Node>* condition = $4;
				tree<ATNN::Node>* body = $6;

				elif->add_child(*condition);
				elif->add_child(*body);
				$$ = elif;
			}
		;

// else
else 	: // nothing
			{
				ATNN::Node astn(new ASTN(L"ELSE"));
				$$ = new tree<ATNN::Node>(astn);
			}
		| ELSE block_instructions
			{
				ATNN::Node astn(new ASTN(L"ELSE"));
				tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* body = $2;

				t->add_child(*body);
				$$ = t;
			}
		;

// while statement
while_stmt	: WHILE OPARENTHESIS expr CPARENTHESIS block_instructions
				{
					ATNN::Node astn(new ASTN(L"WHILE"));
					tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* condition = $3;
					tree<ATNN::Node>* body = $5;

					t->add_child(*condition);
					t->add_child(*body);
					$$ = t;
				}
			;

// for statement
for_stmt	: FOR OPARENTHESIS init SEMICOLON expr SEMICOLON incremental CPARENTHESIS block_instructions
				{
					ATNN::Node astn(new ASTN(L"FOR"));
					tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* init = $3;
					tree<ATNN::Node>* condition = $5;
					tree<ATNN::Node>* final = $7;
					tree<ATNN::Node>* body = $9;

					t->add_child(*init);
					t->add_child(*condition);
					t->add_child(*final);
					t->add_child(*body);
					$$ = t;
				}
			;

// assigment or nothing
init 	: // nothing
			{ ATNN::Node astn(new ASTN(L"ASSIGMENT")); $$ = new tree<ATNN::Node>(astn); }
		| assign
			{ $$ = $1; }
		;

// assigment or expr
incremental 	: // nothing
					{ ATNN::Node astn(new ASTN(L"INCREMENTAL")); $$ = new tree<ATNN::Node>(astn); }
				| assign
					{ $$ = $1; }
				| double_arithmetic
					{ $$ = $1; }
				;

// dowhile statement
dowhile_stmt	: DO block_instructions WHILE OPARENTHESIS expr CPARENTHESIS
				{
					ATNN::Node astn(new ASTN(L"DO WHILE"));
					tree<ATNN::Node>* dowhile = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* body = $2;
					tree<ATNN::Node>* condition = $5;

					dowhile->add_child(*body);
					dowhile->add_child(*condition);
					$$ = dowhile;
				}

// Return statement with an expression
return_stmt	: RETURN
				{
					ATNN::Node astn(new ASTN(L"RETURN"));
					$$ = new tree<ATNN::Node>(astn);
				}
			| RETURN expr
				{
					ATNN::Node astn(new ASTN(L"RETURN"));
					tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* expr = $2;

					t->add_child(*expr);
					$$ = t;
				}
			;

// Print statement
print_stmt 	: PRINT print_list
				{
					ATNN::Node astn(new ASTN(L"PRINT"));
					tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* e = $2;

					t->add_child(*e);
					$$ = t;
				}
			;

// list of exprsions to print
print_list 	: PRINTSEPARATOR element_to_print
				{
					ATNN::Node astn(new ASTN(L"PRINT LIST"));
					tree<ATNN::Node>* t = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* e = $2;

					t->add_child(*e);
					$$ = t;
				}
			| print_list PRINTSEPARATOR element_to_print
				{
					tree<ATNN::Node>* t = $1;
					tree<ATNN::Node>* e = $3;

					t->add_child(*e);
					$$ = t;
				}
			;

element_to_print 	: expr
						{ $$ = $1; }
					| ENDL
						{ ATNN::Node astn(new ASTN(L"ENDL")); $$ = new tree<ATNN::Node>(astn); }
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
				ATNN::Node astn(new ASTN(L"TOKEN BOOL", b));
				$$ = new tree<ATNN::Node>(astn);
			}
		| INT
			{
				ATNN::Node astn(new ASTN(L"TOKEN INT", $1));
				$$ = new tree<ATNN::Node>(astn);
			}
		| DOUBLE
			{
				ATNN::Node astn(new ASTN(L"TOKEN DOUBLE", $1));
				$$ = new tree<ATNN::Node>(astn);
			}
		| STRING
			{
				string s = $1.substr(1, $1.length() - 2);
				ATNN::Node astn(new ASTN(L"TOKEN STRING", converter.from_bytes(s)));
				$$ = new tree<ATNN::Node>(astn);
			}
		| ID
			{
				ATNN::Node astn(new ASTN(L"TOKEN ID", converter.from_bytes($1)));
				$$ = new tree<ATNN::Node>(astn);
			}
		| ID DOT ID
			{
				ATNN::Node astn1(new ASTN(L"OBJECT ACCES"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn1);
				ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);
				ATNN::Node astn3(new ASTN(L"POSITION", converter.from_bytes($3)));
				tree<ATNN::Node>* pos = new tree<ATNN::Node>(astn3);

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		| ID OBRACKET expr CBRACKET
			{
				ATNN::Node astn1(new ASTN(L"ARRAY ACCES"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn1);
				ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
				tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);
				tree<ATNN::Node>* pos = $3;

				n->add_child(*id);
				n->add_child(*pos);
				$$ = n;
			}
		| OBRACER object_list CBRACER
			{
				ATNN::Node astn(new ASTN(L"TOKEN OBJECT"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* list = $2;

				n->add_child(*list);
				$$ = n;
			}
		| OBRACKET expr_list CBRACKET
			{
				ATNN::Node astn(new ASTN(L"TOKEN ARRAY"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* list = $2;

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
				ATNN::Node astn(new ASTN(L"NOT"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* expr = $2;

				n->add_child(*expr);
				$$ = n;
			}
		| MINUS expr %prec NEG
			{
				ATNN::Node astn(new ASTN(L"NEG"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* expr = $2;

				n->add_child(*expr);
				$$ = n;
			}
		| PLUS expr %prec POS
			{
				ATNN::Node astn(new ASTN(L"POS"));
				tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* expr = $2;

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
							ATNN::Node astn(new ASTN(L"PLUS"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>();
							tree<ATNN::Node>* expr1 = $1;
							tree<ATNN::Node>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr MINUS expr
						{
							ATNN::Node astn(new ASTN(L"MINUS"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>();
							tree<ATNN::Node>* expr1 = $1;
							tree<ATNN::Node>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr MULT expr
						{
							ATNN::Node astn(new ASTN(L"MULT"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>();
							tree<ATNN::Node>* expr1 = $1;
							tree<ATNN::Node>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr DIV expr
						{
							ATNN::Node astn(new ASTN(L"DIV"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>();
							tree<ATNN::Node>* expr1 = $1;
							tree<ATNN::Node>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					| expr MOD expr
						{
							ATNN::Node astn(new ASTN(L"MOD"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>();
							tree<ATNN::Node>* expr1 = $1;
							tree<ATNN::Node>* expr2 = $3;

							n->add_child(*expr1);
							n->add_child(*expr2);
							$$ = n;
						}
					;

// Boolean expr
boolean_expr 	: expr EQUAL expr
					{
						ATNN::Node astn(new ASTN(L"EQUAL"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr NOT_EQUAL expr
					{
						ATNN::Node astn(new ASTN(L"NOT_EQUAL"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr LT expr
					{
						ATNN::Node astn(new ASTN(L"LT"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr LE expr
					{
						ATNN::Node astn(new ASTN(L"LE"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr GT expr
					{
						ATNN::Node astn(new ASTN(L"GT"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr GE expr
					{
						ATNN::Node astn(new ASTN(L"GE"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr AND expr
					{
						ATNN::Node astn(new ASTN(L"AND"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				| expr OR expr
					{
						ATNN::Node astn(new ASTN(L"OR"));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* expr1 = $1;
						tree<ATNN::Node>* expr2 = $3;

						n->add_child(*expr1);
						n->add_child(*expr2);
						$$ = n;
					}
				;

// local functions
local_functions : ID DOT LOCALFUNCTION OPARENTHESIS expr_list CPARENTHESIS
					{
						ATNN::Node astn1(new ASTN(L"LOCAL FUNCTION", converter.from_bytes($3)));
						tree<ATNN::Node>* n = new tree<ATNN::Node>(astn1);
						ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
						tree<ATNN::Node>* id = new tree<ATNN::Node>(astn2);
						tree<ATNN::Node>* list = $5;

						n->add_child(*id);
						n->add_child(*list);
						$$ = n;
					}
				;

// double plus or double minus
double_arithmetic 	: DPLUS ids
						{
							ATNN::Node astn(new ASTN(L"DPLUS"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
							tree<ATNN::Node>* e = $2;

							n->add_child(*e);
							$$ = n;
						}
					|  ids DPLUS
						{
							ATNN::Node astn(new ASTN(L"DPLUSR"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
							tree<ATNN::Node>* e = $1;

							n->add_child(*e);
							$$ = n;
						}
					| DMINUS ids
						{
							ATNN::Node astn(new ASTN(L"DMINUS"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
							tree<ATNN::Node>* e = $2;

							n->add_child(*e);
							$$ = n;
						}
					| ids DMINUS
						{
							ATNN::Node astn(new ASTN(L"DMINUSR"));
							tree<ATNN::Node>* n = new tree<ATNN::Node>(astn);
							tree<ATNN::Node>* e = $1;

							n->add_child(*e);
							$$ = n;
						}
					;

// A function call has a lits of arguments in parenthesis (possibly empty)
funcall : ID OPARENTHESIS expr_list CPARENTHESIS
			{
				ATNN::Node astn(new ASTN(L"FUNCALL", converter.from_bytes($1)));
				tree<ATNN::Node>* funcall = new tree<ATNN::Node>(astn);
				tree<ATNN::Node>* list = $3;

				funcall->add_child(*list);
				$$ = funcall;
			}
		;

// A list of expressions separated by commas
expr_list	: // nothing
				{
					ATNN::Node astn(new ASTN(L"EXPR LIST"));
					$$ = new tree<ATNN::Node>(astn);
				}
			| expr
				{
					ATNN::Node astn(new ASTN(L"EXPR LIST"));
					tree<ATNN::Node>* list = new tree<ATNN::Node>(astn);
					tree<ATNN::Node>* expr = $1;

					list->add_child(*expr);
					$$ = list;
				}
			| expr_list COMMA expr
				{
					tree<ATNN::Node>* list = $1;
					tree<ATNN::Node>* expr = $3;

					list->add_child(*expr);
					$$ = list;
				}
			;

// A list of declarations of the items of a dictionary
object_list 	: // nothing
					{
						ATNN::Node astn(new ASTN(L"OBJECT LIST"));
						$$ = new tree<ATNN::Node>(astn);
					}
				| ID COLON expr
					{
						ATNN::Node astn1(new ASTN(L"OBJECT LIST"));
						tree<ATNN::Node>* t = new tree<ATNN::Node>(astn1);
						ATNN::Node astn2(new ASTN(L"ID", converter.from_bytes($1)));
						tree<ATNN::Node>* obj = new tree<ATNN::Node>(astn2);
						tree<ATNN::Node>* body = $3;

						obj->add_child(*body);
						t->add_child(*obj);
						$$ = t;
					}
				| object_list COMMA ID COLON expr
					{
						tree<ATNN::Node>* t = $1;
						ATNN::Node astn(new ASTN(L"ID", converter.from_bytes($3)));
						tree<ATNN::Node>* obj = new tree<ATNN::Node>(astn);
						tree<ATNN::Node>* body = $5;

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