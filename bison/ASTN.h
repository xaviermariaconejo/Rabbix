#ifndef ASTN_H
#define ASTN_H

#include <string>

namespace ATN {

/**
 * AST node (ASTN)
 */
	class ASTN {
		public:
			// Value type
			enum ASTtype { VOID, BOOL, INT, DOUBLE, WSTRING };

			// constructors
			ASTN(const std::wstring& token, bool value);
			ASTN(const std::wstring& token, int value);
			ASTN(const std::wstring& token, double value);
			ASTN(const std::wstring& token, const std::wstring& value);
			ASTN(const std::wstring& token);
			ASTN();

			// copy
			ASTN(const ASTN& astn);

			// assignment
			ASTN& operator=(const ASTN& astn);

			// destructor
			~ASTN();

			// clear the content
			void clear();

			// set bool value
			void setValueBool(bool value);

			// set int value
			void setValueInt(int value);

			// set double value
			void setValueDouble(double value);

			// set string value
			void setValueWstring(const std::wstring& value);

			// get token info
			std::wstring getToken() const;

			// get value type
			ASTtype getType() const;

			// get bool value
			bool getValueBool() const;

			// get int value
			int getValueInt() const;

			//get double value
			double getValueDouble() const;

			//get string value
			std::wstring getValueWstring() const;

		private:
	    	// auxiliary to copy, assignment, and destructor
      		void clone(const ASTN&);

			// information of the token
			std::wstring token;
			
			// value type
			ASTtype type;
			
			// value in case is bool
			bool value_bool;
			
			// value in case is int
			int value_int;
			
			// value in case is double
			double value_double;
			
			// value in case is string
			std::wstring value_string;
	};

}

#endif // ASTN_H