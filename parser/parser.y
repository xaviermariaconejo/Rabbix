/* --Parser-- */

%scanner 					../lexer/Scanner.ih
%scanner-token-function 	d_scanner.lex()

%union
{
	// int 			i;
	double			d;
	// std::wstring	s;
	// void*		p;
}

%token <d>				NUM
//%token <u_symbol>		ID
%type  <d>				exp

//%right '='
%left	PLUS MINUS
%left	MULT DIV
%left	NEG
%right	POW


%%


input:
	// empty
|
	input line
;

line:
	'\n'
|
	exp '\n'
    {
		std::cout << "\t EXP: " << $1 << '\n';
    }
|
	error '\n'
;

exp:
	NUM
	{
		$$ = std::stoi(d_scanner.matched());
		std::cout << "\t NUM: " << $$ << '\n';
	}
    /*
|
	ID
	{
		$$ = *$1;
		std::cout << "\t ID: " << $$ << '\n';
    }
|
	ID '=' exp
	{
		$$ = d_scanner.matched() = $3;
		std::cout << "\t ID = exp: " << $$ << '\n';
	}
    */
|
	exp PLUS exp
	{
		$$ = $1 + $3;
		std::cout << "\t PLUS: " << $1 << " + " << $3 << " = " << $$ << '\n';
	}
|
	exp MINUS exp
	{
		$$ = $1 - $3;
		std::cout << "\t MINUS: " << $1 << " - " << $3 << " = " << $$ << '\n';
	}
|
	exp MULT exp
	{
		$$ = $1 * $3;
		std::cout << "\t MULT: " << $1 << " * " << $3 << " = " << $$ << '\n';
	}
|
	exp DIV exp
	{
		$$ = $1 / $3;
		std::cout << "\t DIV: " << $1 << " / " << $3 << " = " << $$ << '\n';
	}
|
	MINUS exp %prec NEG
	{
		$$ = -$2;
		std::cout << "\t NEG: " << $2 << " = " << $$ << '\n';
	}
|
	exp POW exp
	{
		$$ = -1;
		std::cout << "\t POW: " << $1 << " ^ " << $3 << " = " << $$ << '\n';
	}
|
	'(' exp ')'
	{
		$$ = $2;
		std::cout << "\t ( exp ): " << $2 << " = " << $$ << '\n';
	}
;