#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stack>
#include <list>
#include <map>

#include "Data.h"

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
            ~Interpreter();

            /**
             * Run parser. Results are stored inside.
             * \returns 0 on success, 1 on failure
             */
            int parse();
            
            /**
             * Run interpreter. Results are stored inside.
             */
            void run();

            /**
             * Clear AST
             */
            void clear();
            
            /**
             * Print AST, element could be the id of a global variable, function or ATN
             */
            std::string str() const;
            
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
            /**
             * Used internally to print the AST.
             */
            std::stringstream ASTPrint(const freeling::tree<ASTN>& t, string tab) const;

            /**
             * Used internally by Parser to insert AST nodes.            
             */
            void addMainElement(std::wstring ws, const freeling::tree<ASTN>& t);

            /**
             * Used internally by Scanner YY_USER_ACTION to update location indicator.
             */
            void increaseLocation(unsigned int loc);

            /**
             * Used internally by Scanner to update location indicator
             */
            void increaseLocationRow();
            
            /**
             * Used to get last Scanner location. Used in error messages.
             */
            unsigned int location() const;

            /**
             * Used to get last Scanner column. Used in error messages.
             */
            unsigned int column() const;

            /**
             * Used to get last Scanner row. Used in error messages.
             */
            unsigned int row() const;

            /**
             * Executes a atn.
             */
            void executeAtn(std::wstring atnName);

            /**
             * Executes a function.
             */
            Data executeFunction(std::wstring funcname, const freeling::tree<ASTN>& args);

            /**
             * Gathers the list of arguments of a function call. It also checks
             * that the arguments are compatible with the parameters. In particular,
             * it checks that the number of parameters is the same and that no
             * expressions are passed as parametres by reference.
             */
            list<Data> listArguments(const freeling::tree<ASTN>& AstF, const freeling::tree<ASTN>& args);

            /**
             * Executes a block of instructions. The block is terminated
             * as soon as an instruction returns a non-null result.
             * Non-null results are only returned by "return" statements.
             */
            Data executeListInstructions(const freeling::tree<ASTN>& t);

            /**
             * Executes an instruction. 
             * Non-null results are only returned by "return" statements.
             */
            Data executeInstruction(const freeling::tree<ASTN>& t);

            /**
             * Evaluates the expression represented in the AST t.
             */
            Data evaluateExpression(const freeling::tree<ASTN>& t);
            
            /**
             * Evaluation of Boolean expressions. This function implements
             * a short-circuit evaluation. The second operand is still a tree
             * and is only evaluated if the value of the expression cannot be
             * determined by the first operand.
             */
            Data& evaluateBool(std::wstring type, Data& v, const freeling::tree<ASTN>& t);
            
            /**
             * Checks that the data is Boolean and raises an exception if it is not.
             */
            void checkBool(const Data& b);
    
           /**
            * Checks that the data is integer or double and raises an exception if it is not.
            */
            void checkNumeric(const Data& b);

            
            Scanner m_scanner;
            Parser m_parser;
            unsigned int m_row, m_column, m_location;               // Used by scanner

            std::map<std::wstring, freeling::tree<ASTN> > m_map;    // Map of global variables & functions & ATN's
            std::stack< std::map<std::wstring, Data> > m_stack;     // Stack of variables for each State

            /* Idea */
            // Number of (global variables & functions & ATN's) of all the program, to differentiate from the local global varaibles & fucntions of the actual ATN
    };
    
}

#endif // INTERPRETER_H