#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <list>
#include "tree.h"
#include "ASTN.h"

#include "scanner.h"
#include "parser.hpp"

namespace ATN {

    // forward declare our simplistic AST node class so we
    // can declare container for it without the header
    //class ASTN;

    /**
     * This class is the interface for our scanner/lexer. The end user
     * is expected to use this. It drives scanner/lexer, keeps
     * parsed AST. Both parser and lexer have access to it via internal 
     * references.
     */
    class Interpreter
    {
        public:
            Interpreter();
            
            /**
             * Run parser. Results are stored inside.
             * \returns 0 on success, 1 on failure
             */
            int parse();
            
            /**
             * Clear AST
             */
            void clear();
            
            /**
             * Print AST, element could be the id of a global variable, function or ATN
             */
            std::string str(std::wstring element = L"all") const;
            
            /**
             * Switch scanner input stream. Default is standard input (std::cin).
             * It will also reset AST.
             */
            void switchInputStream(std::istream *is);

            /**
             * This is needed so that Scanner and Parser can call some
             * methods that we want to keep hidden from the end user.
             */
            friend class Parser;
            friend class Scanner;
            
        private:
            // Used internally to print the AST.
            std::stringstream ASTPrint(const freeling::tree<ASTN>& t, string tab) const;

            // Used internally by Parser to insert AST nodes.            
            void addMainElement(const freeling::tree<ASTN>& t);

            // Used internally by Scanner YY_USER_ACTION to update location indicator.
            void increaseLocation(unsigned int loc);

            // Used internally by Scanner to update location indicator
            void increaseLocationRow();
            
            // Used to get last Scanner location. Used in error messages.
            unsigned int location() const;

            // Used to get last Scanner column. Used in error messages.
            unsigned int column() const;

            // Used to get last Scanner row. Used in error messages.
            unsigned int row() const;
            
            Scanner m_scanner;
            Parser m_parser;
            std::list< freeling::tree<ASTN> > m_list;           // Example AST
            unsigned int m_row, m_column, m_location;           // Used by scanner
    };
    
}

#endif // INTERPRETER_H
