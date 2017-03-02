#include "Stack.h"
#include <sstream>

using namespace std;
using namespace ATN;

Stack::Stack() :
	m_stack(),
	m_current(),
	m_trace()
{ }

Stack::Stack(const Stack& s) :
	m_stack(s.m_stack),
	m_current(s.m_current),
	m_trace(s.m_trace)
{ }

Stack::Stack& operator=(const Stack& stack) {
	if (this != &stack) {
      clear();
      clone(stack);
    }
    return (*this);
}

void Stack::clone(const Stack& s) {
	m_stack(s.m_stack);
	m_current(s.m_current);
	m_trace(m_trace);
}

Stack::~Stack() {
	clear();
}

void Stack::clear() {
	m_stack.clear();
	m_current.clear();
	m_trace.clear();
}

void Stack::pushActivationRecord(wstring name, int line) {
    m_current = map<wstring,Data>();
    m_stack.push(m_current);
    m_trace.push(StackTraceItem(name, line));
}

void Stack::popActivationRecord() {
    m_stack.pop();
    if (m_stack.empty()) m_current = nullptr;
    else m_current = m_stack.top();
    m_trace.pop();
}

void Stack::defineVariable(wstring name, const Data& value) {
    m_current[name] = value;
}

Data& Stack::getVariable(wstring name) const {
	Data v = m_current.find(name)->second;
    if (v == nullptr) {
        throw throw runtime_error("Variable " + string(name.begin(), name.end()) + " not defined");
    }
    return v;
}
const Data& Stack::getVariable(wstring name) const {
	Data v = m_current.find(name)->second;
    if (v == nullptr) {
        throw throw runtime_error("Variable " + string(name.begin(), name.end()) + " not defined");
    }
    return v;
}

std::wstring Stack::getStackTrace(int current_line) {
	int size = m_trace.size();
    stack<StackTraceItem> itr(m_trace);
    stringstream trace << "---------------" << endl << "| Stack trace |" << endl << "---------------" << endl;
    trace << "** Depth = " << size << endl;
    while (!itr.empty()) {
        StackTraceItem it = itr.pop();
        trace << "|> " << it.fname << ": line " << current_line << endl;
        current_line = it.line;
    }
    string s = trace.toString();
    return wstring(s.begin(), s.end());
}

std::wstring Stack::getStackTrace(int current_line, int nitems) {
	int size = m_trace.size();
    if (2*nitems >= size) return getStackTrace(current_line);
    stack<StackTraceItem> itr(m_trace);
    stringstream trace << "---------------" << endl << "| Stack trace |" << endl << "---------------" << endl;
    trace << "** Depth = " << size << endl;
    int i;
    for (i = 0; i < nitems; ++i) {
       StackTraceItem it = itr.pop();
       trace << "|> " << it.fname << ": line " << current_line << endl;
       current_line = it.line;
    }
    trace << "|> ..." << endl;
    for (; i < size-nitems; ++i) current_line = itr.pop().line;
    for (; i < size; ++i) {
       StackTraceItem it = itr.pop();
       trace << "|> " << it.fname << ": line " << current_line << endl;
       current_line = it.line;
    }
    string s = trace.toString();
    return wstring(s.begin(), s.end());
}