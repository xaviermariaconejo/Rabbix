#include "Data.h"

#include <assert.h>

using namespace std;
using namespace ATN;

Data::Data() :
	type(VOID),
	value_bool(),
	value_double(),
	value_wstring(),
	value_array(),
	value_map()
{ }

Data::Data(bool b) :
	type(VOID),
	value_bool(b),
	value_double(),
	value_wstring(),
	value_array(),
	value_map()
{ }

Data::Data(double x) :
	type(VOID),
	value_bool(),
	value_double(x),
	value_wstring(),
	value_array(),
	value_map()
{ }

Data::Data(wstring ws) :
	type(VOID),
	value_bool(),
	value_double(),
	value_wstring(ws),
	value_array(),
	value_map()
{ }

Data::Data(const vector<Data>& v) :
	type(VOID),
	value_bool(),
	value_double(),
	value_wstring(),
	value_array(v),
	value_map()
{ }

Data::Data(const map<wstring, Data>& m) :
	type(VOID),
	value_bool(),
	value_double(),
	value_wstring(),
	value_array(),
	value_map(m)
{ }

Data::Data(const Data& data) {
	clone(data);
}

void Data::clone(const Data& n) {
	type = n.type;
	value_bool = n.value_bool;
	value_double = n.value_double;
	value_wstring(n.value_wstring);
	value_array(n.value_array);
	value_map(n.value_map);
}

Data::~Data() {
	clear();
}

void Data::clear() {
	type = VOID;
	value_bool();
	value_double();
	value_wstring();
	value_array.clear();
	value_map.clear();
}

Data::Data& operator=(const Data& data) {
	if (this != &data) {
      clear();
      clone(data);
    }
    return (*this);
}

Data::DataType Data::getType() {
	return type;
}

bool Data::isVoid() {
	return type == VOID;
}

bool Data::isBool() {
	return type == BOOL;
}

bool Data::isDouble() {
	return type == DOUBLE;
}

bool Data::isWstring() {
	return type == WSTRING;
}

bool Data::isArray() {
	return type == ARRAY;
}
		
bool Data::isMap() {
	return type == MAP;
}
			
bool Data::getBoolValue() const {
	assert(type == BOOL);
    return value_bool;
}

double Data::getDoubleValue() const {
	assert(type == DOUBLE);
    return value_double;
}

std::wstring Data::getWstringValue() const {
	assert(type == WSTRING);
    return value_wstring;
}

std::vector<Data>& Data::getArrayValue() const {
	assert(type == ARRAY);
    return value_array;
}

const std::vector<Data>& Data::getArrayValue() const {
	assert(type == ARRAY);
    return value_array;
}

Data& Data::getArrayValue(int i) const {

}

const Data& Data::getArrayValue(int i) const {

}

Data& Data::getIndexOfArray(const Data& d) const {

}

const Data& Data::getIndexOfArray(const Data& d) const {

}

int Data::getSizeArray() const {

}

std::map<std::wstring, Data>& Data::getMapValue() const {
	assert(type == MAP);
	return value_map;
}
const std::map<std::wstring, Data>& Data::getMapValue() const {
	assert(type == MAP);
	return value_map;
}

Data& Data::getMapValue(std::wstring ws) const {

}

const Data::Data& getMapValue(std::wstring ws) const {

}

Data& Data::getMapValue(int i) const {

}

const Data::Data& getMapValue(int i) const {

}

int Data::getSizeMap() const {

}

void Data::setBoolValue(bool b) {
	type = BOOL;
	value_bool = b;
}

void Data::setDoubleValue(double d) {
	type = DOUBLE;
	value_double = d;
}

void Data::setWstringValue(wstring ws) {
	type = WSTRING;
	value_wstring = ws;
}

void Data::setArrayValue(const vector<Data>& v) {
	type = ARRAY;
	value_array = v;
}

void Data::addArrayValue(const Data& d) {

}

void Data::addArrayValue(int i, const Data& d) {

}

void Data::deleteArrayValue(int i) {

}

void Data::setMapValue(const map<wstring, Data>& m) {
	type = MAP;
	value_map = m;
}

void Data::addMapValue(std::wstring ws, const Data& d) {

}

void Data::deleteMapValue(std::wstring ws) {

}

wstring Data::toString() const {
	assert(type != VOID && type != ARRAY && type != MAP);
	switch (type) {
		case BOOL: return value_bool ? L"TRUE" : L"FALSE";
		case DOUBLE: return to_wstring(value_double);
		case WSTRING: return value_wstring;
		default: assert(false);
	}
}

void Data::evaluateArithmetic (wstring op, const Data& d) {
	assert(type == DOUBLE && d.type == DOUBLE);
    switch (op) {
        case L"PLUS": value_double += d.value_double; break;
        case L"MINUS": value_double -= d.value_double; break;
        case L"MUL": value_double *= d.value_double; break;
        case L"DIV": checkDivZero(d); value_double /= d.value_double; break;
        case L"MOD": checkDivZero(d); value_double %= d.value_double; break;
        default: assert(false);
    }
}

void Data::checkDivZero(const Data& d) {
	if (d.value_double == 0) throw runtime_error("division by zero");
}

Data& Data::evaluateRelational (wstring op, Data d) {
	assert(type != VOID && type != ARRAY && type != MAP && type == d.type);
	if (type == BOOL) value_wstring = value_bool ? L"1" : L"0"; 
	if (type == DOUBLE) value_wstring = to_wstring(value_double);
	if (type == BOOL || type == DOUBLE || type == WSTRING) {
		switch (op) {
        case L"EQUAL": return new Data(value_wstring == d.value_wstring);
        case L"NOT_EQUAL": return new Data(value_wstring != d.value_wstring);
        case L"LT": return new Data(value_wstring < d.value_wstring);
        case L"LE": return new Data(value_wstring <= d.value_wstring);
        case L"GT": return new Data(value_wstring > d.value_wstring);
        case L"GE": return new Data(value_wstring >= d.value_wstring);
        default: assert(false;) 
    }
    return null;
}

const Data& Data::evaluateRelational (wstring op, Data d) {
	assert(type != VOID && type != ARRAY && type != MAP && type == d.type);
	if (type == BOOL) value_wstring = value_bool ? L"1" : L"0"; 
	if (type == DOUBLE) value_wstring = to_wstring(value_double);
	if (type == BOOL || type == DOUBLE || type == WSTRING) {
		switch (op) {
	    case L"EQUAL": return new Data(value_wstring == d.value_wstring);
	    case L"NOT_EQUAL": return new Data(value_wstring != d.value_wstring);
	    case L"LT": return new Data(value_wstring < d.value_wstring);
	    case L"LE": return new Data(value_wstring <= d.value_wstring);
	    case L"GT": return new Data(value_wstring > d.value_wstring);
	    case L"GE": return new Data(value_wstring >= d.value_wstring);
	    default: assert(false;) 
	}
	return null;
}