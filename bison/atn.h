#ifndef ATN_H
#define ATN_H

#include <locale>
#include <codecvt>
#include <stack>
#include <list>
#include <map>

#include "data.h"

#include "tree.h"
#include "astn.h"
#include "atnn.h"

#include "scanner.h"
#include "parser.hpp"

namespace ATN {

    // forward declare our simplistic AST node class so we
    // can declare container for it without the header
    // class ASTN;

    /**
     * This class is the interface for our scanner/lexer. The end user
     * is expected to use this. It drives scanner/lexer, keeps
     * parsed AST. Both parser and lexer have access to it via internal 
     * references.
     */
    class Atn
    {
        public:

            /**
             *
             */
            struct Output
            {
                Output() { }
                Output(int i, int j): init(i), final(j) { }
                Output(int i, int j, std::wstring ws): init(i), final(j), info(ws) { }

                int init;
                int final;
                std::wstring info;
            };

            Atn();
            Atn(std::wstring name);
            ~Atn();
            
            /**
             *
             */
            int Atn::parse()

            /**
             * Switch scanner input stream. Default is standard input (std::cin).
             * It will also reset AST.
             */
            void switchInputStream(std::istream *is);
            
            /**
             * Run interpreter. Results are stored inside.
             */
            std::vector<Output> run(const std::vector<std::wstring>& in);

            /**
             * Clear AST
             */
            void clear();
            
            /**
             * Print AST, element could be the id of a global variable, function or ATN
             */
            std::string str();

            /**
             * This is needed so that Scanner and Parser can call some
             * methods that we want to keep hidden from the end user.
             */
            friend class Parser;
            friend class Scanner;
            
        private:
            /**
             * Run parser. Results are stored inside.
             * \returns 0 on success, 1 on failure
             */
            std::stringstream ASTPrint(const freeling::tree<ASTN*>& t, string tab);
            std::stringstream ASTPrint(const ATNN& atn, string tab);

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
             * Executes an atn.
             */
            vector<Output> executeAtn(std::wstring atnname, const std::vector<std::wstring>& in, int init, int act);

            /**
             * Executes a state of an atn.
             */
            vector<Atn::Output> executeState(std::wstring atnname, const std::vector<std::wstring>& in, const freeling::tree<ASTN*>& state, int init, int act, std::map<std::wstring, Data*> global, const std::vector<std::wstring>& finals, const std::map<std::wstring, freeling::tree<ASTN*>*>& states, bool final);

            /**
             * Executes a function.
             */
            Data* executeFunction(std::wstring funcname, const freeling::tree<ASTN*>::const_iterator& args, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input);

            /**
             * Gathers the list of arguments of a function call. It also checks
             * that the arguments are compatible with the parameters. In particular,
             * it checks that the number of parameters is the same and that no
             * expressions are passed as parametres by reference.
             */
            std::map<std::wstring, Data*> listArguments(const freeling::tree<ASTN*>::const_iterator& AstF, const freeling::tree<ASTN*>::const_iterator& args, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input);

            /**
             * Executes a block of instructions. The block is terminated
             * as soon as an instruction returns a non-null result.
             * Non-null results are only returned by "return" statements.
             */
            Data* executeListInstructions(const freeling::tree<ASTN*>::const_iterator& t, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, bool final);

            /**
             * Executes an instruction. 
             * Non-null results are only returned by "return" statements.
             */
            Data* executeInstruction(const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, bool final);

            /**
             * Evaluates the expression represented in the AST t.
             */
            Data* evaluateExpression(const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input);

            /**
             * Get de pointer Data of the TOKEN ID, ARRAY ACCES or OBJECT ACCES
             * used in assigmanets, double arithmetic and andpersand
             */
            Data* getAccesData(const ASTN& t, const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input);
            
            /**
             * Evaluation of Boolean expressions. This function implements
             * a short-circuit evaluation. The second operand is still a tree
             * and is only evaluated if the value of the expression cannot be
             * determined by the first operand.
             */
            Data* evaluateBool(std::wstring type, Data* v, const freeling::tree<ASTN*>::const_iterator& t, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input);
           
            /**
             * Checks that the data is Boolean and raises an exception if it is not.
             */
            void checkBool(const Data* b) const;
    
           /**
            * Checks that the data is integer or double and raises an exception if it is not.
            */
            void checkNumeric(const Data* b) const;

            /**
             * Prepare the string to print, check special characters
             */
            void printOutput(std::string s) const;

            /**
             * Find the element in the vector
             */
            int find(const std::vector<std::wstring>& v, std::wstring ws) const;

            
            wstring_convert< codecvt_utf8_utf16<wchar_t> > converter;               // Converer wstring - string

            Scanner m_scanner;
            Parser m_parser;
            unsigned int m_row, m_column, m_location;                               // Used by scanner

            std::map<std::wstring, freeling::tree<ASTN*>* > m_func;                 // Map of functions
            std::map<std::wstring, ATNN* > m_atn;                                   // Map of ATN's
            std::map<std::wstring, Data* > m_global;                                // Map of global variables
    };
    
}

#endif // ATN_H