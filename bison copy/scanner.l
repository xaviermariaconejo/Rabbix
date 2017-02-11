%option noyywrap

%{
	#include <iostream>
	#include "parser.tab.h"

	#define YY_DECL extern "C" int yylex()
	using namespace std;

	int line_num = 0;
%}

ws			[ \t\v\f]+
bool		"true"|"TRUE"|"false"|"FALSE"
dig			[0-9]
alpha		[a-zA-Z_]
alphanum	[a-zA-Z0-9_]
string		(\"(\\.|[^\\"])*\"|\'(\\.|[^\\'])*\')


%%


"if"		{ yylval.sval = strdup(yytext); return IF; }
"else"		{ yylval.sval = strdup(yytext); return ELSE; }
"while"		{ yylval.sval = strdup(yytext); return WHILE; }
"do"		{ yylval.sval = strdup(yytext); return DO; }
"for"		{ yylval.sval = strdup(yytext); return FOR; }
"function"	{ yylval.sval = strdup(yytext); return FUNC; }
"return"	{ yylval.sval = strdup(yytext); return RETURN; }
"global"	{ yylval.sval = strdup(yytext); return GLOBAL; }


","					{ yylval.sval = strdup(yytext); return COMMA; }
";"					{ yylval.sval = strdup(yytext); return SEMICOLON; }
":"					{ yylval.sval = strdup(yytext); return COLON; }
"&"					{ yylval.sval = strdup(yytext); return ANDPERSAND; }
"="					{ yylval.sval = strdup(yytext); return ASSIGMENT; }

"("					{ yylval.sval = strdup(yytext); return OPARENTHESIS; }
")"					{ yylval.sval = strdup(yytext); return CPARENTHESIS; }
"["					{ yylval.sval = strdup(yytext); return OBRACKET; }
"]"					{ yylval.sval = strdup(yytext); return CBRACKET; }
"{"					{ yylval.sval = strdup(yytext); return OBRACER; }
"}"					{ yylval.sval = strdup(yytext); return CBRACER; }

"+"					{ yylval.sval = strdup(yytext); return PLUS; }
"-"					{ yylval.sval = strdup(yytext); return MINUS; }
"*"					{ yylval.sval = strdup(yytext); return MULT; }
"/"					{ yylval.sval = strdup(yytext); return DIV; }
"%"					{ yylval.sval = strdup(yytext); return MOD; }
"^"					{ yylval.sval = strdup(yytext); return POW; }

"=="				{ yylval.sval = strdup(yytext); return EQUAL; }
"!="				{ yylval.sval = strdup(yytext); return NOT_EQUAL; }
"<"					{ yylval.sval = strdup(yytext); return LT; }
"<="				{ yylval.sval = strdup(yytext); return LE; }
">"					{ yylval.sval = strdup(yytext); return GT; }
">="				{ yylval.sval = strdup(yytext); return GE; }

"not"|"NOT"|"!"		{ yylval.sval = strdup(yytext); return NOT; }
"and"|"AND"|"&&"	{ yylval.sval = strdup(yytext); return AND; }
"or"|"OR"|"||"		{ yylval.sval = strdup(yytext); return OR; }


{bool} 				{ yylval.sval = strdup(yytext); return BOOL; }
{dig}+ 				{ yylval.ival = atoi(yytext); return INT; }
{dig}*\.{dig}* 		{ yylval.fval = atof(yytext); return DOUBLE; }
{string} 			{ yylval.sval = strdup(yytext); return STRING; }
{alpha}+{alphanum}*	{ yylval.sval = strdup(yytext); return ID; }


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
.		{ cout << "NOT DEFINED: " << yytext << endl; yylval.sval = strdup(yytext); return NOT_DEFINED; }
%%