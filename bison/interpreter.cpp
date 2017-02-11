#include "interpreter.h"

#include <sstream>
//#include <boost/concept_check.hpp>

using namespace ATN;

Interpreter::Interpreter() :
    m_list(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_column(0),
    m_row(1),
    m_location(0)
{

}

int Interpreter::parse() {
    m_column = m_location = 0;
    m_row = 1;
    return m_parser.parse();
}

void Interpreter::clear() {
    m_column = m_location = 0;
    m_row = 1;
    m_list.clear();
}

std::string Interpreter::str() const {
    std::stringstream s;
    s << "Interpreter: " << " commands received from command line." << endl;
    /*for(int i = 0; i < m_commands.size(); i++) {
        s << " * " << m_commands[i].str() << endl;
    }*/
    return s.str();
}

void Interpreter::switchInputStream(std::istream *is) {
    m_scanner.switch_streams(is, NULL);
    m_list.clear();    
}

void Interpreter::addMainElement(const freeling::tree<ASTN>& t)
{
    m_list.push_back(t);
}

void Interpreter::increaseLocation(unsigned int loc) {
    m_location += loc;
    m_column += loc;
}

void Interpreter::increaseLocationRow() {
    m_row++;
    m_column = 0;
}

unsigned int Interpreter::location() const {
    return m_location;
}

unsigned int Interpreter::column() const {
    return m_column;
}

unsigned int Interpreter::row() const {
    return m_row;
}
