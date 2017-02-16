#include "ASTN.h"

#include <iostream>

using namespace ATN;

// clone
void ASTN::clone(const ASTN& n) {
	token = n.token;
	type = n.type;
	value_bool = n.value_bool;
	value_int = n.value_int;
	value_double = n.value_double;
	value_string = n.value_string;
}

// constructors
ASTN::ASTN() :
	token(L""),
	type(VOID)
{ }

ASTN::ASTN(const std::wstring& t) :
	token(t),
	type(VOID)
{ }

ASTN::ASTN(const std::wstring& t, bool value) :
	token(t),
	value_bool(value),
	type(BOOL)
{ }

ASTN::ASTN(const std::wstring& t, int value) :
	token(t),
	value_int(value),
	type(INT)
{ }

ASTN::ASTN(const std::wstring& t, double value) :
	token(t),
	value_double(value),
	type(DOUBLE)
{ }

ASTN::ASTN(const std::wstring& t, const std::wstring& value) :
	token(t),
	value_string(value),
	type(WSTRING)
{ }

//copy
ASTN::ASTN(const ASTN& n) {
	clone(n);
}

// assignment
ASTN& ASTN::operator=(const ASTN& n) {
	if (this != &n) {
      clear();
      clone(n);
    }
    return (*this);
}

// destructor
ASTN::~ASTN() {
	clear();
}

// clear content
void ASTN::clear() {
	token = value_string = L"";
	value_bool = value_int = value_double = 0;
}

// set bool value
void ASTN::setValueBool(bool b) {
	type = BOOL;
	value_bool = b;
}

// set int value
void ASTN::setValueInt(int i) {
	type = INT;
	value_int = i;
}

// set double value
void ASTN::setValueDouble(double d) {
	type = DOUBLE;
	value_double = d;
}

// set string value
void ASTN::setValueWstring(const std::wstring& s) {
	type = WSTRING;
	value_string = s;
}

// get token info
std::wstring ASTN::getToken() const {
	return token;
}

ASTN::ASTtype ASTN::getType() const {
	return type;
}

// get bool value
bool ASTN::getValueBool() const {
	if (type != BOOL) {
		throw std::runtime_error("the type of the ASTN is not correct");
	}
	else {
		return value_bool;
	}
}

// get int value
int ASTN::getValueInt() const {
	if (type != INT) {
		throw std::runtime_error("the type of the ASTN is not correct");
	}
	else {
		return value_int;
	}
}

// get double value
double ASTN::getValueDouble() const {
	if (type != DOUBLE) {
		throw std::runtime_error("the type of the ASTN is not correct");
	}
	else {
		return value_double;
	}
}

// get string value
std::wstring ASTN::getValueWstring() const {
	if (type != WSTRING) {
		throw std::runtime_error("the type of the ASTN is not correct");
	}
	else {
		return value_string;
	}
}