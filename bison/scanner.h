#ifndef SCANNER_H
#define SCANNER_H


#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer ATN_FlexLexer
#include <FlexLexer.h>
#endif

// Scanner method signature is defined by this macro. Original yylex() returns int.
// Since Bison 3 uses symbol_type, we must change returned type. We also rename it
// to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL ATN::Parser::symbol_type ATN::Scanner::get_next_token()

#include "parser.hpp" // this is needed for symbol_type

namespace ATN {

	// Forward declare interpreter to avoid include. Header is added in implementation file.
	class Interpreter; 
	    
	class Scanner : public yyFlexLexer {
		public:
	        Scanner(Interpreter &driver) : m_driver(driver) {}
			virtual ~Scanner() {}
			virtual ATN::Parser::symbol_type get_next_token();
		        
		private:
		    Interpreter &m_driver;
	};

}

#endif