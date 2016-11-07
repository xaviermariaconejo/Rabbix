/* --Lexer-- */

%class-name	Scanner

%%

'='									return Parser::EQUAL;
'+'									return Parser::PLUS;
'-'									return Parser::MINUS;
'*'									return Parser::MULT;
'/'									return Parser::DIV;
'^'									return Parser::POW;
'('									return Parser::OPAR;
')'									return Parser::CPAR;

"true"|"TRUE"|"false"|"FALSE"		return Parser::BOOL;
[[:digit:]]*						return Parser::INT;
[[:digit:]]*(','|'.')[[:digit:]]*	return Parser::DOUBLE;
[[:alpha:]_][[:alnum:]_]*			return Parser::ID;
<<EOF>>								return Parser::EOF;
'\n'								return '\n';
' '|'\t'
