#include "interpreter.h"

#include <sstream>
//#include <boost/concept_check.hpp>

using namespace std;
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

string Interpreter::str(wstring element) const {
    stringstream s;
    string elem(element.begin(), element.end());
    s << "Interpreter: " << "Printing " << elem << " element/s" << endl << endl;
    bool found = false;

    for (auto it = m_list.begin(); it != m_list.end() && !found; ++it) {
        freeling::tree<ASTN> t = *it;
        freeling::tree<ASTN>::const_iterator itTree = t.begin();
        wstring ws = (*itTree).getValueWstring();
        string token(ws.begin(), ws.end());

        if (token == elem || elem == "all") {
            found = elem != "all";
            s << ASTPrint(t, "").str() << endl;
        }
    }

    return s.str();
}

void Interpreter::switchInputStream(istream *is) {
    m_scanner.switch_streams(is, NULL);
    m_list.clear();    
}

stringstream Interpreter::ASTPrint(const freeling::tree<ASTN>& t, string tab) const {
    stringstream s;

    freeling::tree<ASTN>::const_iterator it = t.begin();
    ASTN node = *it;
    wstring ws = node.getToken();
    s << string(ws.begin(), ws.end());

    ASTN::ASTtype type = node.getType();
    if (type == ASTN::ASTtype::BOOL) {
        if (node.getValueBool()) {
            s << ": TRUE";
        }
        else {
            s << ": FALSE";
        }
    }
    else if (type == ASTN::ASTtype::INT) {
        s << ": " + to_string(node.getValueInt());
    }
    else if (type == ASTN::ASTtype::DOUBLE) {
        s << ": " + to_string(node.getValueDouble());
    }
    else if (type == ASTN::ASTtype::WSTRING) {
        ws = node.getValueWstring();
        s << ": " + string(ws.begin(), ws.end());
    }
    s << endl;

    tab += "     |";
    for (int i = 0; i < t.num_children(); ++i) {
        if (i == t.num_children() - 1) {
            string tabAux = tab.substr(0, tab.length() - 1);
            s << tab << endl << tabAux << "'--> " << ASTPrint(t.nth_child_ref(i), tabAux).str();
        }
        else {
            s << tab << endl << tab << "--> " << ASTPrint(t.nth_child_ref(i), tab).str();            
        }
    }

    return s;
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
