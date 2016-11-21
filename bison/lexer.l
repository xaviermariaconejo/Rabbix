%option noyywrap

%{
	#include <iostream>
	#include "parser.tab.h"

	#define YY_DECL extern "C" int yylex()
	using namespace std;

	int line_num = 0;
%}

ws			[ \r\t]+
bool		"true"|"TRUE"|"false"|"FALSE"
dig			[0-9]
alpha		[a-zA-Z_]
alphanum	[a-zA-Z0-9_]
string		\"(\\.|[^\\"])*\"


%%


"if"		return IF;
"else"		return ELSE;
"while"		return WHILE;
"do"		return DO;
"for"		return FOR;
"function"	return FUNC;
"return"	return RETURN;
"global"	return GLOBAL;


","					return COMMA;
";"					return SEMICOLON;
":"					return COLON;
"&"					return ANDPERSAND;
"="					return ASSIGMENT;

"("					return OPARENTHESIS;
")"					return CPARENTHESIS;
"["					return OBRACKET;
"]"					return CBRACKET;
"{"					return OBRACER;
"}"					return CBRACER;

"+"					return PLUS;
"-"					return MINUS;
"*"					return MULT;
"/"					return DIV;
"%"					return MOD;
"^"					return POW;

"=="				return EQUAL;
"!="				return NOT_EQUAL;
"<"					return LT;
"<="				return LE;
">"					return GT;
">="				return GE;

"not"|"NOT"|"!"		return NOT;
"and"|"AND"|"&&"	return AND;
"or"|"OR"|"||"		return OR;


{bool} 				{ yylval.sval = strdup(yytext); return BOOL; }
{dig}+ 				{ yylval.ival = atoi(yytext); return INT; }
{dig}*\.{dig}* 		{ yylval.fval = atof(yytext); return DOUBLE; }
{string} 			{ yylval.sval = strdup(yytext); return STRING; }
{alpha}+{alphanum}*	{ yylval.sval = strdup(yytext); return ID; }


.		{ yylval.sval = strdup(yytext); /*return NOT_DEFINED; */}
\n 		{ ++line_num; }
{ws}	{ /* nothing */ }
"/*"	{
			/* nothing */
			int c;

			while((c = yyinput()) != 0)
			{
				if(c == '\n')
					++line_num;

				else if(c == '*')
				{
					if((c = yyinput()) == '/')
						break;
					else
						unput(c);
				}
			}
		}
%%