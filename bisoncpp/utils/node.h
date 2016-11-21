#ifndef _NODE_
#define _NODE_

#include <string>

	class node {
		private:
	    	// auxiliary to copy, assignment, and destructor
      		void clone(const node&);

		protected:
			// information of the token
			std::wstring token;
			// value in case is bool
			bool value_bool;
			// value in case is int
			int value_int;
			// value in case is double
			double value_double;
			// value in case is string
			std::wstring value_string;

		public:
			// constructor
			node();
			// copy
			node(const node&);
			// assignment
			node& operator=(const node&);
			// destructor
			~node();

			// clear the content
			void clear();

			// set token info
			void setToken(const std::wstring&);
			// set bool value
			void setValueBool(bool);
			// set int value
			void setValueInt(int);
			// set double value
			void setValueDouble(double);
			// set string value
			void setValueString(const std::wstring&);

			// get token info
			std::wstring getToken();
			// get bool value
			bool getValueBool();
			// get int value
			int getValueInt();
			//get double value
			double getValueDouble();
			//get string value
			std::wstring getValueString();
	};

	// clone
	void node::clone(const node& n) {
		token = n.token;
		value_bool = n.value_bool;
		value_int = n.value_int;
		value_double = n.value_double;
		value_string = n.value_string;
	}

	// constructor
	node::node() { }

	//copy
	node::node(const node& n) {
		clone(n);
	}

	// assignment
	node& node::operator=(const node& n) {
		if (this != &n) {
	      clear();
	      clone(n);
	    }
	    return (*this);
	}

	// destructor
	node::~node() {
		clear();
	}

	// clear content
	void node::clear() {
		token = value_string = L"";
		value_bool = value_int = value_double = 0;
	}

	// set token info
	void node::setToken(const std::wstring& s) {
		token = s;
	}

	// set bool value
	void node::setValueBool(bool b) {
		value_bool = b;
	}

	// set int value
	void node::setValueInt(int i) {
		value_int = i;
	}

	// set double value
	void node::setValueDouble(double d) {
		value_double = d;
	}

	// set string value
	void node::setValueString(const std::wstring& s) {
		value_string = s;
	}

	// get token info
	std::wstring node::getToken() {
		return token;
	}

	// get bool value
	bool node::getValueBool() {
		return value_bool;
	}

	// get int value
	int node::getValueInt() {
		return value_int;
	}

	// get double value
	double node::getValueDouble() {
		return value_double;
	}

	// get string value
	std::wstring node::getValueString() {
		return value_string;
	}

#endif