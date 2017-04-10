#include "atnn.h"

using namespace ATN;

void ATNN::clone(const ATNN& n) {
	m_name = n.m_name;
	m_initials = n.m_initials;
	m_finals = n.m_finals;
	m_states = n.m_states;
}

ATNN::ATNN() :
	m_name(),
	m_initials(),
	m_finals(),
	m_states()
{ }

ATNN::ATNN(std::wstring name) :
	m_name(name),
	m_initials(),
	m_finals(),
	m_states()
{ }

ATNN::ATNN(const ATNN& n) {
	clone(n);
}

ATNN& ATNN::operator=(const ATNN& n) {
	if (this != &n) {
      clear();
      clone(n);
    }
    return (*this);
}

ATNN::~ATNN() {
	clear();
}

void ATNN::clear() {
	m_name = L"";
	m_initials.clear();
	m_finals.clear();
	m_states.clear();
}

void ATNN::setName(std::wstring name) {
	m_name = name;
}

void ATNN::setInitials(const std::vector<std::wstring>& v) {
	m_initials = v;
}

void ATNN::setFinals(const std::vector<std::wstring>& m) {
	m_finals = m;
}

void ATNN::setStates(const std::map<std::wstring, freeling::tree<ASTN*>*>& m) {
	m_states = m;
}

std::wstring ATNN::getName() const {
	return m_name;
}

std::vector<std::wstring>& ATNN::getInitials() {
	return m_initials;
}

const std::vector<std::wstring>& ATNN::getInitials() const {
	return m_initials;
}

std::vector<std::wstring>& ATNN::getFinals() {
	return m_finals;
}

const std::vector<std::wstring>& ATNN::getFinals() const {
	return m_finals;
}

std::map<std::wstring, freeling::tree<ASTN*>*>& ATNN::getStates() {
	return m_states;
}

const std::map<std::wstring, freeling::tree<ASTN*>*>& ATNN::getStates() const {
	return m_states;
}