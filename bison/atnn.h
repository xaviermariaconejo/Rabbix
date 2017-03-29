#ifndef ATNN_H
#define ATNN_H

#include <iostream>
#include <vector>
#include <map>
#include "tree.h"
#include "astn.h"

namespace ATN {

/**
 * ATN Node (ASTNN)
 */
	class ATNN
	{
		public:
			struct Node {
				Node(): astn(nullptr), atn(nullptr) { }
				Node(ASTN* n): astn(n), atn(nullptr) { }
				Node(ATNN* n): astn(nullptr), atn(n) { }
		    	ASTN* astn;
		    	ATNN* atn;
		    };

			// constructors
			ATNN();
			ATNN(std::wstring name);

			// copy
			ATNN(const ATNN& atnn);

			// assignment
			ATNN& operator=(const ATNN& atnn);

			// destructor
			~ATNN();

			// clear the content
			void clear();

			// set name of the atn
			void setName(std::wstring name);

			// set initials
			void setInitials(const std::vector<std::wstring>& v);

			// set finals
			void setFinals(const std::map<std::wstring, freeling::tree<Node>*>& m);

			// set states
			void setStates(const std::map<std::wstring, freeling::tree<Node>*>& m);

      		//get name of the atn
      		std::wstring getName() const;

			// get value initials
			std::vector<std::wstring>& getInitials();
			const std::vector<std::wstring>& getInitials() const;

			// get value finals
			std::map<std::wstring, freeling::tree<Node>*>& getFinals();
			const std::map<std::wstring, freeling::tree<Node>*>& getFinals() const;

			//get value states
			std::map<std::wstring, freeling::tree<Node>*>& getStates();
			const std::map<std::wstring, freeling::tree<Node>*>& getStates() const;

		private:
	    	// auxiliary to copy, assignment, and destructor
      		void clone(const ATNN& n);

      		// name of the atn
      		std::wstring m_name;

      		// vector of initials states
      		std::vector<std::wstring> m_initials;

			// map finals & states
			std::map<std::wstring, freeling::tree<Node>*> m_finals, m_states;
	};

}

#endif // ATNN_H