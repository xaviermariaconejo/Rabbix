/* --Lexer-- */

%class-name	Scanner

%%


// KEY WORDS
"if"								return Parser::IF;
"else"								return Parser::ELSE;
"while"								return Parser::WHILE;
"do"								return Parser::DO;
"for"								return Parser::FOR;
"function"							return Parser::FUNC;
"return"							return Parser::RETURN;
"global"							return Parser::GLOBAL;


// OPERATIONS
','									return Parser::COMMA;
';'									return Parser::SEMICOLON;
':'									return Parser::COLON;
'&'									return Parser::ANDPERSAND;
'='									return Parser::ASSIGMENT;

'+'									return Parser::PLUS;
'-'									return Parser::MINUS;
'*'									return Parser::MULT;
'/'									return Parser::DIV;
'%'									return Parser::MOD;
'^'									return Parser::POW;

'=='								return Parser::EQUAL;
'!='								return Parser::NOT_EQUAL;
'<'									return Parser::LT;
LT ASSIGMENT						return Parser::LE;
'>'									return Parser::GT;
LE ASSIGMENT						return Parser::GE;

"not"|"NOT"|"!"						return Parser::NOT;
"and"|"AND"|"&&"					return Parser::AND;
"or"|"OR"|"||"						return Parser::OR;

'"'|"'"								return Parser::QUOTE;
'('									return Parser::OPARENTHESIS;
')'									return Parser::CPARENTHESIS;
'['									return Parser::OBRACKET;
']'									return Parser::CBRACKET;
'{'									return Parser::OBRACER;
'}'									return Parser::CBRACER;


// TYPES
"true"|"TRUE"|"false"|"FALSE"		return Parser::BOOL;
[[:digit:]]+						return Parser::INT;
[[:digit:]]*(','|'.')[[:digit:]]+	return Parser::DOUBLE;
[[:alpha:]_][[:alnum:]_]*			return Parser::ID;


// White spaces
'\n'|'\r'|'\t'|' '					// nothing
