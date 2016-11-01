/* --Parser-- */

%scanner 					../lexer/Scanner.ih
%scanner-token-function 	d_scanner.lex()
%baseclass-preinclude		cmath

%union
{
	int 			i;
	double			d;
	// std::wstring	s;
	// void*		p;
}

%token <i>				BOOL
%token <i>				INT
%token <d>				DOUBLE
//%token <u_symbol>		ID

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
		std::cout << "\t EXP: " << $1.i << " " << $1.d << '\n';
    }
|
	error '\n'
;

exp:
	INT
	{
		$$.i = std::stoi(d_scanner.matched());
		std::cout << "\t INT: " << $$ << '\n';
	}
|
	DOUBLE
	{
		$$.d = std::stod(d_scanner.matched());
		std::cout << "\t DOUBLE: " << $$ << '\n';
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
		$$.i = $1.i + $3.i;
		$$.d = $1.d + $3.d;
		std::cout << "\t PLUS: " << $1 << " + " << $3 << " = " << $$ << '\n';
	}
|
	exp MINUS exp
	{
		$$.i = $1.i - $3.i;
		$$.d = $1.d - $3.d;
		std::cout << "\t MINUS: " << $1 << " - " << $3 << " = " << $$ << '\n';
	}
|
	exp MULT exp
	{
		$$.i = $1.i * $3.i;
		$$.d = $1.d * $3.d;
		std::cout << "\t MULT: " << $1 << " * " << $3 << " = " << $$ << '\n';
	}
|
	exp DIV exp
	{
		$$.i = $1.i / $3.i;
		$$.d = $1.d / $3.d;
		std::cout << "\t DIV: " << $1 << " / " << $3 << " = " << $$ << '\n';
	}
|
	MINUS exp %prec NEG
	{
		$$.i = -$2.i;
		$$.d = -$2.d;
		std::cout << "\t NEG: " << $2 << " = " << $$ << '\n';
	}
|
	exp POW exp
	{
		$$.i = pow($1.i, $3.i);
		$$.d = pow($1.d, $3.d);
		std::cout << "\t POW: " << $1 << " ^ " << $3 << " = " << $$ << '\n';
	}
	/*
|
	'(' exp ')'
	{
		$$ = $2;
		std::cout << "\t ( exp ): " << $2 << " = " << $$ << '\n';
	}
	*/
;