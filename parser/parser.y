/* --Parser-- */

%scanner 					../lexer/Scanner.ih
%scanner-token-function 	d_scanner.lex()
//%parse-param				{int* n}

%union
{
	tree<node>* t;
}

%token 					OPAR CPAR EOF
%token 	<t>				BOOL INT DOUBLE ID exp

%right 	EQUAL
%left	PLUS MINUS
%left	MULT DIV
%left	NEG
%right	POW


%%


// A program is a list of main elements (ATN's, states, functions, global variables)
prog	: mainElement+ EOF
        ;

// A main element is an element as the type of: ATN, State, Function, Global variables
mainElement	: //atn
		//	| state
			| func
		//	| global

// A function has a name, a list of parameters and a block of instructions	
func	: FUNC ID params block_instructions ENDFUNC!
        ;

input:
	// empty
|
	input line
	{
		global = 3;
		std::cout << "GLOBAL: " << global << "\n";
	}
;

line:
	'\n'
|
	exp '\n'
    {
		std::cout << "\t EXP: " << $1 << " " << $1.i << " " << $1.d << " " << $1.p << " "  << '\n';
    }
|
	error '\n'
;

exp:
	INT
	{
		$$.i = stoi(d_scanner.matched());
		std::cout << "\t INT: " << $$.i << '\n';
	}
|
	DOUBLE
	{
		$$.d = stod(d_scanner.matched());
		std::cout << "\t DOUBLE: " << $$.d << '\n';
	}
|
	ID
	{
		$$.p = static_cast<void*>(new std::string(d_scanner.matched()));
		void* vp = static_cast<void*>(new std::string(d_scanner.matched()));
		std::string* sp = static_cast<std::string*>(vp);
		std::string s = *sp;
		std::cout << "\t ID: " << s << '\n';
    }
    /*
|
	ID EQUAL exp
	{
		$$.p = d_scanner.matched();
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
		$$ = $1.i * $3.i;
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
	OPAR exp CPAR
	{
		$$ = $2;
		std::cout << "\t ( exp ): " << $2 << " = " << $$ << '\n';
	}
;