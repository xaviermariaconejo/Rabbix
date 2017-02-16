#ifndef STACK_H
#define STACK_H

#include <queue>
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
		    void pushActivationRecord();

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

		private:
    		// Stack of activation records
		    std::queue< std::map<std::wstring, Data> > m_stack;
		    
		    // Reference to the current activation record
		    std::map<std::wstring, Data> m_current;
	};
}

#endif 	//STACK_H