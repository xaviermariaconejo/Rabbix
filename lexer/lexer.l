/* --Lexer-- */

%class-name	Scanner

%%

//'='								return '=';
'+'									return Parser::PLUS;
'-'									return Parser::MINUS;
'*'									return Parser::MULT;
'/'									return Parser::DIV;
'^'									return Parser::POW;
'('									return Parser::OPAR;
')'									return Parser::CPAR;
[[:digit:]]*						return Parser::NUM;
//[[:alpha:]_][[:alnum:]_]*			return Parser::ID;
'\n'								return '\n';
' '
