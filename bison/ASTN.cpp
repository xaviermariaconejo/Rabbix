#include "astn.h"

#include <iostream>
#include <assert.h>

using namespace ATN;

void ASTN::clone(const ASTN& n) {
	token = n.token;
	type = n.type;
	value_bool = n.value_bool;
	value_int = n.value_int;
	value_double = n.value_double;
	value_string = n.value_string;
}

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

ASTN::ASTN(const ASTN& n) {
	clone(n);
}

ASTN& ASTN::operator=(const ASTN& n) {
	if (this != &n) {
      clear();
      clone(n);
    }
    return (*this);
}

ASTN::~ASTN() {
	clear();
}

void ASTN::clear() {
	token = value_string = L"";
	value_bool = value_int = value_double = 0;
}

void ASTN::setValueBool(bool b) {
	type = BOOL;
	value_bool = b;
}

void ASTN::setValueInt(int i) {
	type = INT;
	value_int = i;
}

void ASTN::setValueDouble(double d) {
	type = DOUBLE;
	value_double = d;
}

void ASTN::setValueWstring(const std::wstring& s) {
	type = WSTRING;
	value_string = s;
}

std::wstring ASTN::getToken() const {
	return token;
}

ASTN::ASTtype ASTN::getType() const {
	return type;
}

bool ASTN::getValueBool() const {
	assert(type == BOOL);
	return value_bool;
}

int ASTN::getValueInt() const {
	assert(type == INT);
	return value_int;
}

double ASTN::getValueDouble() const {
	assert(type == DOUBLE);
	return value_double;
}

std::wstring ASTN::getValueWstring() const {
	assert(type == WSTRING);
	return value_string;
}