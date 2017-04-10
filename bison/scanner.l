%{
	#include <iostream>
	#include <cstdlib>
	#include "scanner.h"
	#include "atn.h"
	#include "parser.hpp"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() ATN::Parser::make_END(ATN::location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION m_driver.increaseLocation(yyleng);
%}


%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="ATN_"

ws				[ \t\v\f]+
bool			"true"|"TRUE"|"false"|"FALSE"
dig				[0-9]
alpha			[a-zA-Z_]
alphanum		[a-zA-Z0-9_]
string			(\"(\\.|[^\\"])*\"|\'(\\.|[^\\'])*\')
local_function 	"add"|"remove"|"indexOf"|"size"|"substring"|"lema"


%%


"print"				{ return ATN::Parser::make_PRINT(ATN::location()); }
"<<"				{ return ATN::Parser::make_PRINTSEPARATOR(ATN::location()); }
"if"				{ return ATN::Parser::make_IF(ATN::location()); }
"else"				{ return ATN::Parser::make_ELSE(ATN::location()); }
"else if"			{ return ATN::Parser::make_ELSEIF(ATN::location()); }
"while"				{ return ATN::Parser::make_WHILE(ATN::location()); }
"do"				{ return ATN::Parser::make_DO(ATN::location()); }
"for"				{ return ATN::Parser::make_FOR(ATN::location()); }
"function"			{ return ATN::Parser::make_FUNC(ATN::location()); }
"return"			{ return ATN::Parser::make_RETURN(ATN::location()); }
"global"			{ return ATN::Parser::make_GLOBAL(ATN::location()); }
"endl"				{ return ATN::Parser::make_ENDL(ATN::location()); }

"atn"|"ATN"															{ return ATN::Parser::make_ATN(ATN::location()); }
"state"|"STATE"														{ return ATN::Parser::make_STATE(ATN::location()); }
"initial states"|"initial STATES"|"INITIAL states"|"INITIAL STATES"	{ return ATN::Parser::make_INITIAL(ATN::location()); }
"final states"|"final STATES"|"FINAL states"|"FINAL STATES"			{ return ATN::Parser::make_FINAL(ATN::location()); }
"action"|"ACTION"													{ return ATN::Parser::make_ACTION(ATN::location()); }
"transition"|"TRANSITION"											{ return ATN::Parser::make_TRANSITION(ATN::location()); }
"to"|"TO"															{ return ATN::Parser::make_TO(ATN::location()); }

"."					{ return ATN::Parser::make_DOT(ATN::location()); }
","					{ return ATN::Parser::make_COMMA(ATN::location()); }
";"					{ return ATN::Parser::make_SEMICOLON(ATN::location()); }
":"					{ return ATN::Parser::make_COLON(ATN::location()); }
"&"					{ return ATN::Parser::make_ANDPERSAND(ATN::location()); }
"="					{ return ATN::Parser::make_ASSIGMENT(ATN::location()); }

"("					{ return ATN::Parser::make_OPARENTHESIS(ATN::location()); }
")"					{ return ATN::Parser::make_CPARENTHESIS(ATN::location()); }
"["					{ return ATN::Parser::make_OBRACKET(ATN::location()); }
"]"					{ return ATN::Parser::make_CBRACKET(ATN::location()); }
"{"					{ return ATN::Parser::make_OBRACER(ATN::location()); }
"}"					{ return ATN::Parser::make_CBRACER(ATN::location()); }

"+"					{ return ATN::Parser::make_PLUS(ATN::location()); }
"-"					{ return ATN::Parser::make_MINUS(ATN::location()); }
"*"					{ return ATN::Parser::make_MULT(ATN::location()); }
"/"					{ return ATN::Parser::make_DIV(ATN::location()); }
"%"					{ return ATN::Parser::make_MOD(ATN::location()); }
"++"				{ return ATN::Parser::make_DPLUS(ATN::location()); }
"--"				{ return ATN::Parser::make_DMINUS(ATN::location()); }


"=="				{ return ATN::Parser::make_EQUAL(ATN::location()); }
"!="				{ return ATN::Parser::make_NOT_EQUAL(ATN::location()); }
"<"					{ return ATN::Parser::make_LT(ATN::location()); }
"<="				{ return ATN::Parser::make_LE(ATN::location()); }
">"					{ return ATN::Parser::make_GT(ATN::location()); }
">="				{ return ATN::Parser::make_GE(ATN::location()); }

"not"|"NOT"|"!"		{ return ATN::Parser::make_NOT(ATN::location()); }
"and"|"AND"|"&&"	{ return ATN::Parser::make_AND(ATN::location()); }
"or"|"OR"|"||"		{ return ATN::Parser::make_OR(ATN::location()); }


{bool} 						{ return ATN::Parser::make_BOOL(strdup(yytext), ATN::location()); }
{dig}+ 						{ return ATN::Parser::make_INT(atoi(yytext), ATN::location()); }
{dig}*\.{dig}* 				{ return ATN::Parser::make_DOUBLE(atof(yytext), ATN::location()); }
{string} 					{ return ATN::Parser::make_STRING(strdup(yytext), ATN::location()); }
{alpha}+{alphanum}*|"$"|"@"	{ return ATN::Parser::make_ID(strdup(yytext), ATN::location()); }
{local_function} 			{ return ATN::Parser::make_LOCALFUNCTION(strdup(yytext), ATN::location()); }


\n 		{ m_driver.increaseLocationRow(); }
{ws}	{ cout << "HI THERE " << m_driver.row() << " " << m_driver.column() << endl;/* nothing */ }
"/*"	{
			/* nothing */
			int c;

			while((c = yyinput()) != 0)
			{
				if(c == '\n') {
					m_driver.increaseLocationRow();
				}

				else if(c == '*') {
					if((c = yyinput()) == '/') break;
					else unput(c);
				}
			}
		}
.		{ cout << "NOT DEFINED: -" << yytext << "-" << endl; return ATN::Parser::make_NOT_DEFINED(ATN::location()); }
<<EOF>>	{ return yyterminate(); }


%%