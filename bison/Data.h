#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <vector>
#include <map>

namespace ATN {
/**
 * Class to represent data in the interpreter.
 * Each data item has a type and a value. The type can be bool,
 * double, wstring, array or map. Each operation asserts 
 * hat the operands have the appropriate types.
 * All the arithmetic and Boolean operations are calculated in-place,
 * i.e., the result is stored in the same data.
 * The type VOID is used to represent void values on function returns.
 */
	class Data
	{
		public:
			// value type
			enum DataType { VOID, BOOL, DOUBLE, WSTRING, ARRAY, MAP };

			// constructors
			Data();
			Data(bool b);
			Data(double x);
			Data(std::wstring ws);
			Data(const std::vector<Data>& v);
			Data(const std::map<std::wstring, Data>& m);

			// copy
			Data(const Data& data);

			// assignment
			Data& operator=(const Data& data);

			// destructor
			~Data();

			// clear the content
			void clear();

		    // Returns the type of data
    		DataType getType();

		    // Indicates whether the data is void
			bool isVoid();

		    // Indicates whether the data is bool
			bool isBool();

		    // Indicates whether the data is double
			bool isDouble();

		    // Indicates whether the data is wstring
			bool isWstring();

		    // Indicates whether the data is array
			bool isArray();
		
		    // Indicates whether the data is map
			bool isMap();
			
			/**
		     * Gets the value of an bool data. The method asserts that
		     * the data is an bool.
		     */
			bool getBoolValue() const;

			/**
		     * Gets the value of an double data. The method asserts that
		     * the data is an double.
		     */
			double getDoubleValue() const;

			/**
		     * Gets the value of an wstring data. The method asserts that
		     * the data is an wstring.
		     */
			std::wstring getWstringValue() const;

			/**
		     * Gets the value of an array data. The method asserts that
		     * the data is an array.
		     */
			std::vector<Data>& getArrayValue() const;
			const std::vector<Data>& getArrayValue() const;

			/**
		     * Gets the value of an map data. The method asserts that
		     * the data is an map.
		     */
			std::map<std::wstring, Data>& getMapValue() const;
			const std::map<std::wstring, Data>& getMapValue() const;

		    // Defines a bool value for the data
			void setBoolValue(bool b);

		    // Defines a double value for the data
			void setDoubleValue(double d);

		    // Defines a wstring value for the data
			void setWstringValue(std::wstring ws);

		    // Defines a array value for the data
			void setArrayValue(const std::vector<Data>& v);

		    // Defines a map value for the data
			void setMapValue(const std::map<std::wstring, Data>& m);

		    // Returns a string representing the data in textual form.
			std::wstring toString() const;

		    // Evaluation of arithmetic expressions. Returning the result on the same data.
		    void evaluateArithmetic (std::wstring op, const Data& d);

	        // Evaluation of expressions with relational operators.
			Data& evaluateRelational (std::wstring op, Data d);
			const Data& evaluateRelational (std::wstring op, Data d);

		private:
		    /**
		     * Checks for zero (for division). It raises an exception in case
		     * the value is zero.
		     */
			void checkDivZero(const Data& d);

	    	// auxiliary to copy, assignment, and destructor
      		void clone(const Data& data);

			// Type of the data
			DataType type;

			// The value of the data
			bool value_bool;
			double value_double;
			std::wstring value_wstring;
			std::vector<Data> value_array;
			std::map<std::wstring, Data> value_map;
    }
}

#endif 	//DATA_H