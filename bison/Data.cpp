#include "data.h"

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
	type(BOOL),
	value_bool(b),
	value_int(),
	value_double(),
	value_wstring(),
	value_array(),
	value_map()
{ }

Data::Data(int n) :
	type(INT),
	value_bool(),
	value_int(n),
	value_double(),
	value_wstring(),
	value_array(),
	value_map()
{ }

Data::Data(double x) :
	type(DOUBLE),
	value_bool(),
	value_int(),
	value_double(x),
	value_wstring(),
	value_array(),
	value_map()
{ }

Data::Data(wstring ws) :
	type(WSTRING),
	value_bool(),
	value_int(),
	value_double(),
	value_wstring(ws),
	value_array(),
	value_map()
{ }

Data::Data(const vector<Data*>& v) :
	type(ARRAY),
	value_bool(),
	value_int(),
	value_double(),
	value_wstring(),
	value_array(v),
	value_map()
{ }

Data::Data(const map<wstring, Data*>& m) :
	type(MAP),
	value_bool(),
	value_int(),
	value_double(),
	value_wstring(),
	value_array(),
	value_map(m)
{ }

Data::Data(const Data* data) {
	clone(data);
}

void Data::clone(const Data* n) {
	type = n->type;
	value_bool = n->value_bool;
	value_int = n->value_int;
	value_double = n->value_double;
	value_wstring = wstring(n->value_wstring);
	value_array = vector<Data*>(n->value_array);
	value_map = map<wstring, Data*>(n->value_map);
}

Data::~Data() {
	clear();
}

void Data::clear() {
	type = VOID;
	value_bool = false;
	value_int = 0;
	value_double = 0.0;
	value_wstring = L"";
	value_array.clear();
	value_map.clear();
}

Data::DataType Data::getType() const {
	return type;
}

bool Data::equals(const Data* d) const {
	assert(type != MAP);
	if (type != d->type) return false;
	else {
		switch (type) {
			case BOOL: return value_bool == d->value_bool;
			case INT: return value_int == d->value_int;
			case DOUBLE: return value_double == d->value_double;
			case WSTRING: return value_wstring == d->value_wstring;
			case ARRAY: if (value_array.size() != d->value_array.size()) return false;
						else {
							for (int i = 0; i < value_array.size(); ++i) {
								if (!value_array[i]->equals(d->value_array[i])) return false;
								else return true;
							}
						}
			default: return false;
		}
	}
}


bool Data::isVoid() const {
	return type == VOID;
}

bool Data::isBool() const {
	return type == BOOL;
}

bool Data::isInt() const {
	return type == INT;
}

bool Data::isDouble() const {
	return type == DOUBLE;
}

bool Data::isWstring() const {
	return type == WSTRING;
}

bool Data::isArray() const {
	return type == ARRAY;
}
		
bool Data::isMap() const {
	return type == MAP;
}
			
bool Data::getBoolValue() const {
	assert(type == BOOL);
    return value_bool;
}

int Data::getIntValue() const {
	assert(type == INT);
    return value_int;
}

double Data::getDoubleValue() const {
	assert(type == DOUBLE);
    return value_double;
}

wstring Data::getWstringValue() const {
	assert(type == WSTRING);
    return value_wstring;
}

wstring Data::getWstringValue(int i) const {
	assert(type == WSTRING);
	return value_wstring.substr(i, i + 1);;
}

int Data::getSizeWstring() const {
	assert(type == WSTRING);
	return value_wstring.size();
}

vector<Data*>& Data::getArrayValue() {
	assert(type == ARRAY);
    return value_array;
}

const vector<Data*>& Data::getArrayValue() const {
	assert(type == ARRAY);
    return value_array;
}

Data* Data::getArrayValue(int i) {
	assert(type == ARRAY);
	return value_array[i];
}

const Data* Data::getArrayValue(int i) const {
	assert(type == ARRAY);
	return value_array[i];
}

int Data::getIndexOfArray(const Data* d) const {
	assert(type == ARRAY);
	for (int i = 0; i < value_array.size(); ++i) {
		if (value_array[i]->equals(d)) return i;
	}
	return -1;
}

int Data::getSizeArray() const {
	assert(type == ARRAY);
	return value_array.size();
}

map<wstring, Data*>& Data::getMapValue() {
	assert(type == MAP);
	return value_map;
}

const map<wstring, Data*>& Data::getMapValue() const {
	assert(type == MAP);
	return value_map;
}

Data* Data::getMapValue(wstring ws) {
	assert(type == MAP);
	return value_map[ws];
}

Data* Data::getMapValue(int i) {
	assert(type == MAP);
	int j = 0;
	for (auto& x: value_map) {
		if (i == j++) return x.second;
	}
	throw runtime_error("out of index");
}

const Data* Data::getMapValue(int i) const {
	assert(type == MAP);
	int j = 0;
	for (auto& x: value_map) {
		if (i == j++) return x.second;
	}
	throw runtime_error("out of index");
}

int Data::getSizeMap() const {
	assert(type == MAP);
	return value_map.size();
}

void Data::setDataValue(const Data* d) {
	clone(d);
}

void Data::setBoolValue(bool b) {
	type = BOOL;
	value_bool = b;
}

void Data::setIntValue(int n) {
	type = INT;
	value_int = n;
}

void Data::setDoubleValue(double d) {
	type = DOUBLE;
	value_double = d;
}

void Data::setWstringValue(wstring ws) {
	type = WSTRING;
	value_wstring = ws;
}

void Data::setArrayValue(const vector<Data*>& v) {
	type = ARRAY;
	value_array = v;
}

void Data::addArrayValue(Data* d) {
	assert(type == ARRAY);
	value_array.push_back(d);
}

void Data::addArrayValue(int i, Data* d) {
	assert(type == ARRAY);
	value_array[i] = d;
}

void Data::insertArrayValue(int i, Data* d) {
	assert(type == ARRAY);
	value_array.insert(value_array.begin() + i, d);
}

void Data::deleteArrayValue(int i) {
	assert(type == ARRAY);
	value_array.erase(value_array.begin() + i);
}

void Data::setMapValue(const map<wstring, Data*>& m) {
	type = MAP;
	value_map = m;
}

void Data::addMapValue(wstring ws, Data* d) {
	assert(type == MAP);
	value_map[ws] = d;
}

void Data::deleteMapValue(wstring ws) {
	assert(type == MAP);
	value_map.erase(ws);
}

wstring Data::toString() const {
	assert(type != VOID && type != ARRAY && type != MAP);
	switch (type) {
		case BOOL: return value_bool ? L"TRUE" : L"FALSE";
		case INT: return to_wstring(value_int);
		case DOUBLE: return to_wstring(value_double);
		case WSTRING: return value_wstring;
		default: assert(false);
	}
}

void Data::evaluateArithmetic (wstring op, Data* d) {
	assert((type == INT || type == DOUBLE) && (d->type == INT || d->type == DOUBLE));
	if (type == INT && d->type == INT) {
		if (op == L"PLUS") value_int += d->value_int;
		else if (op == L"MINUS") value_int -= d->value_int;
		else if (op == L"MULT") value_int *= d->value_int;
		else if (op == L"DIV") { checkDivZero(d); value_int /= d->value_int; }
		else if (op == L"MOD") { checkDivZero(d); value_int %= d->value_int; }
		else assert(false);
	}
	else { //type == DOUBLE || d.type == DOUBLE
		assert(op != L"MOD");

		if (type == INT) value_double = value_int;
		if (d->type == INT) d->value_double = d->value_int;
		type = DOUBLE;

		if (op == L"PLUS") value_double += d->value_double;
		else if (op == L"MINUS") value_double -= d->value_double;
		else if (op == L"MULT") value_double *= d->value_double;
		else if (op == L"DIV") { checkDivZero(d); value_double /= d->value_double; }
		else assert(false);
	}
}

void Data::checkDivZero(const Data* d) {
	assert(d->type == INT || d->type == DOUBLE);
	if (d->type == INT) {
		if (d->value_int == 0) throw runtime_error("division by zero");
	}
	else { // type == DOUBLE
		if (d->value_double == 0) throw runtime_error("division by zero");
	}
}

Data* Data::evaluateRelational (wstring op, Data* d) {
	assert(type != VOID && type != ARRAY && type != MAP);
	double d1, d2;
	if (type == INT) {
		d1 = value_int;
	}
	else if (type == DOUBLE) {
		d1 = value_double;
	}
	if (d->type == INT) {
		d2 = d->value_int;
	}
	else if (d->type == DOUBLE) {
		d2 = d->value_double;
	}

	if ((type == INT || type == DOUBLE) && (d->type == INT || d->type == DOUBLE)) {
		if (op == L"EQUAL") return new Data(d1 == d2);
		else if (op == L"NOT_EQUAL") return new Data(d1 != d2);
		else if (op == L"LT") return new Data(d1 < d2);
		else if (op == L"LE") return new Data(d1 <= d2);
		else if (op == L"GT") return new Data(d1 > d2);
		else if (op == L"GE") return new Data(d1 >= d2);
		else assert(false);
	}
	else {
		wstring ws1, ws2;
		if (type == WSTRING) ws1 = value_wstring;
		else ws1 = to_wstring(d1);
		if (d->type == WSTRING) ws2 = value_wstring;
		else ws2 = to_wstring(d2);

		if (op == L"EQUAL") return new Data(ws1 == ws2);
		else if (op == L"NOT_EQUAL") return new Data(ws1 != ws2);
		else if (op == L"LT") return new Data(ws1 < ws2);
		else if (op == L"LE") return new Data(ws1 <= ws2);
		else if (op == L"GT") return new Data(ws1 > ws2);
		else if (op == L"GE") return new Data(ws1 >= ws2);
		else assert(false);
	}
}