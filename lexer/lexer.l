/* --Lexer-- */

%class-name Scanner

%%

//'='								return '=';
'+'									return Parser::PLUS;
'-'									return Parser::MINUS;
'*'									return Parser::MULT;
'/'									return Parser::DIV;
//'^'								return '^';
//'('								return '(';
//')'								return ')';
[[:digit:]]*						return Parser::NUM;
//[[:alpha:]_][[:alnum:]_]*			return Parser::ID;
'\n'								return '\n';
