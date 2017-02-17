#ifndef STACK_H
#define STACK_H

#include <stack>
#include <map>

#include "Data.h"

namespace ATN {
/**
 * Class to represent the memory of the virtual machine of the
 * interpreter. The memory is organized as a stack of activation
 * records and each entry in the activation record contains is a pair
 * <name of variable, value>.
 */
	class Stack
	{
		public:
		    /**
		     * Class to represent an item of the Stack trace.
		     * For each function call, the function name and
		     * the line number of the call are stored.
		     */
		    class StackTraceItem {
		        public:
		        	std::wstring fname; // Function name
		        	int line; // Line number
		        
		        	// constructors
		        	StackTraceItem() { }
		        	StackTraceItem(std::wstring name, int l) :
		        		fname(name),
		        		line(l)
		        	{ }
		    }

			// constructor
			Stack();

			// copy
			Stack(const Stack& stack);

			// assignment
			Stack& operator=(const Stack& stack);

			// destructor
			~Stack();

			// clear the content
			void clear();

		    // creates a new activation record on the top of the stack
		    void pushActivationRecord(std::wstring name, int line);

		    // destroys the current activation record
		    void popActivationRecord();

			/** Defines the value of a variable. If the variable does not
			 * exist, it is created. If it exists, the value and type of
			 * the variable are re-defined.
			 */
		    void defineVariable(std::wstring name, const Data& value);

		    /** Gets the value of the variable. The value is represented as
		     * a Data object.
		     */
		    Data& getVariable(std::wstring name) const;
		    const Data& getVariable(std::wstring name) const;

		    /**
		     * Generates a wstring with the contents of the stack trace.
		     * Each line contains a function name and the line number where
		     * the next function is called. Finally, the line number in
		     * the current function is written.
		     */ 
		    std::wstring getStackTrace(int current_line);

		    /**
		     * Generates a string with a summarized contents of the stack trace.
		     * Only the first and last items of the stack trace are returned.
		     */ 
		    std::wstring getStackTrace(int current_line, int nitems);

		private:
	    	// auxiliary to copy, assignment, and destructor
      		void clone(const Stack& stack);

    		// Stack of activation records
		    std::stack< std::map<std::wstring, Data> > m_stack;
		    
		    // Reference to the current activation record
		    std::map<std::wstring, Data> m_current;

    		// Stack trace to keep track of function calls
		    std::stack<StackTraceItem> m_trace;
	};
}

#endif 	//STACK_H