#include "atn.h"

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;
using namespace ATN;

Atn::Atn() :
    m_atn(),
    m_func(),
    m_global(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_column(0),
    m_row(1),
    m_location(0)
{ }

Atn::Atn(std::wstring name) :
    m_atn(),
    m_func(),
    m_global(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_column(0),
    m_row(1),
    m_location(0)
{
    ifstream file( converter.to_bytes(name) );
    if (file) {
        switchInputStream(&file);
        parse();
        file.close();
    }
    else {
        throw runtime_error("Error reading the input file: " + converter.to_bytes(name));
    }
}

int Atn::parse() {
    m_column = m_location = 0;
    m_row = 1;
    return m_parser.parse();
}

void Atn::switchInputStream(std::istream *is) {
    m_scanner.switch_streams(is, NULL);
    m_func.clear();
    m_global.clear();
}

std::vector<Atn::Output> Atn::run(const std::vector<std::wstring>& in) {
    map<std::wstring, Data*> copy_global;
    for (auto it = m_global.begin(); it != m_global.end(); ++it) {
        copy_global[it->first] = new Data(*(it->second));
    }
    vector<Atn::OutputInternal> v = executeAtn(L"main", copy_global, in, 0, 0);
    checkOutput(v);

    vector<Atn::Output> out = vector<Atn::Output>(v.size());
    for (int i = 0; i < v.size(); ++i) {
        out[i] = Output(v[i]);
    }
    return out;
}

Atn::~Atn() {
    clear();
}

void Atn::clear() {
    m_column = m_location = 0;
    m_row = 1;
    m_func.clear();
    m_atn.clear();
    m_global.clear();
}

string Atn::str() {
    stringstream s;
    s << "Interpreter: Printing all elements" << endl << endl;
    for (auto it = m_global.begin(); it != m_global.end(); ++it) {
        wstring ws = it->first;
        s << "GLOBAL ID: " << converter.to_bytes(ws) << endl << endl;
    }
    for (auto it = m_atn.begin(); it != m_atn.end(); ++it) {
         ATNN* atn = it->second;
         s << ASTPrint(*atn, "").str() << endl;
    }
    for (auto it = m_func.begin(); it != m_func.end(); ++it) {
         freeling::tree<ASTN*>* t = it->second;
         s << ASTPrint(*t, "").str() << endl;
    }
    return s.str();
}

std::stringstream Atn::ASTPrint(const freeling::tree<ASTN*>& t, std::string tab) {
    stringstream s;
    ASTN node = *(*(t.begin()));
    s << converter.to_bytes(node.getToken());

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
        s << ": " + converter.to_bytes(node.getValueWstring());
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

std::stringstream Atn::ASTPrint(const ATNN& node, std::string tab) {
    stringstream s;
    s << "ATN: " << converter.to_bytes(node.getName()) << endl;
    tab += "     |";
    string tabAct = tab;
    tab += "     |";

    vector<wstring> initials = node.getInitials();
    s << tabAct << endl << tabAct << "--> INITIALS" << endl;
    for (int i = 0; i < initials.size(); ++i) {
        if (i == initials.size() - 1) {
            string tabAux = tab.substr(0, tab.length() - 1);
            s << tab << endl << tabAux << "'--> " << converter.to_bytes(initials[i]) << endl ;
        }
        else {
            s << tab << endl << tab << "--> " << converter.to_bytes(initials[i]) << endl ;         
        }
    }

    vector<wstring> finals = node.getFinals();
    s << tabAct << endl << tabAct << "--> FINALS" << endl;
    for (int i = 0; i < finals.size(); ++i) {
        if (i == finals.size() - 1) {
            string tabAux = tab.substr(0, tab.length() - 1);
            s << tab << endl << tabAux << "'--> " << ASTPrint(finals[i], tabAux).str();
        }
        else {
            s << tab << endl << tab << "--> " << ASTPrint(finals[i], tab).str();            
        }
    }

    map<wstring, tree<ASTN*>*> states = node.getStates();
    s << tabAct << endl << tabAct.substr(0, tabAct.length() - 1) << "'--> STATES" << endl;
    tab = tabAct.substr(0, tabAct.length() - 1) + "     |";
    for (auto it = states.begin(); it != states.end(); ++it) {
        freeling::tree<ASTN*>* state = it->second;
        if (std::next(it) == states.end()) {
            string tabAux = tab.substr(0, tab.length() - 1);
            s << tab << endl << tabAux << "'--> " << ASTPrint(*state, tabAux).str();
        }
        else {
            s << tab << endl << tab << "--> " << ASTPrint(*state, tab).str();            
        }
    }
    return s;
}

void Atn::increaseLocation(unsigned int loc) {
    m_location += loc;
    m_column += loc;
}

void Atn::increaseLocationRow() {
    m_row++;
    m_column = 0;
}

unsigned int Atn::location() const {
    return m_location;
}

unsigned int Atn::column() const {
    return m_column;
}

unsigned int Atn::row() const {
    return m_row;
}



std::vector<Atn::OutputInternal> Atn::executeAtn(std::wstring atnname, std::map<std::wstring, Data*>& copy_global, const std::vector<std::wstring>& in, int init, int act) {
    // Get the data of the ATN
    auto atn_id = m_atn.find(atnname);
    if (atn_id == m_atn.end()) throw runtime_error(" atn " + converter.to_bytes(atnname) + " not declared");
    ATNN atn = *(atn_id->second);

    // Inicializate output
    vector<Atn::OutputInternal> finalOutput;

    // Execute all the initial states at the start of the input
    vector<wstring> initials = atn.getInitials();
    vector<wstring> finals = atn.getFinals();
    map<wstring, tree<ASTN*>*> states = atn.getStates();
    for (int i = 0; i < initials.size(); ++i) {
        wstring stateInit = initials[i];
        auto st = states.find(stateInit);
        if (st == states.end()) throw runtime_error(" state " + converter.to_bytes(stateInit) + " not declared");
        auto stateFinal = std::find(finals.begin(), finals.end(), stateInit);
        vector<Atn::OutputInternal> output = executeState(in, *(st->second), init, act, copy_global, finals, states, stateFinal != finals.end());
        finalOutput.insert( finalOutput.end(), output.begin(), output.end() );
        checkOutput(finalOutput);
    }

    return finalOutput;
}

std::vector<Atn::OutputInternal> Atn::executeState(const std::vector<std::wstring>& in, const freeling::tree<ASTN*>& state, int init, int act, std::map<std::wstring, Data*>& global, const std::vector<std::wstring>& finals, const std::map<std::wstring, freeling::tree<ASTN*>*>& states, bool final) {
    // Inicializate de stack of the state & output
    std::stack< std::map<std::wstring, Data*> > m_stack; std::map<std::wstring, Data*> m;
    m_stack.push(m);
    vector<Atn::OutputInternal> actualOutput;

    // Execute the list of instructions of the state, if
    // the state is final, push the result in the vector
    // of results
    executeListInstructions(((state.nth_child(0)).nth_child(0)).begin(), global, m_stack, in, final);
    if (final) {
        if (m_stack.top().find(L"@") != m_stack.top().end()) {
            Data* value = m_stack.top()[L"@"];
            if (!value->isVoid() && !value->isArray() && !value->isMap()) {
                Atn::OutputInternal newOutput(init, act, value->toString());
                // Copy to mantain consistency
                map<wstring, Data*> copy_global;
                for (auto it = global.begin(); it != global.end(); ++it) {
                    copy_global[it->first] = new Data(*(it->second));
                    newOutput.global[it->first] = new Data(*(it->second));
                }
                actualOutput.push_back(newOutput);
                 if (act < in.size()) {
                    vector<Atn::OutputInternal> output = executeAtn(L"main", copy_global, in, act, act);
                    actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
                    checkOutput(actualOutput);
                }
            }
            else throw runtime_error("The output value in @ has to be a string or a number");
        }
        else throw runtime_error("A final state need a valid string as output in the variable @");
    }

    if (act < in.size()) {
        // Check all the transitions and go to
        // the state if the result is true
        bool atLeastOne = false;
        auto list = state.nth_child(1);
        for (int i = 0; i < list.num_children(); ++i) {
            // Check if is an atn or a expr
            auto it = (list.nth_child(i)).nth_child(1);
            ASTN* node = *(it.begin());
            wstring token = node->getToken();

            // Get the data necessary
            wstring nextState = (*((list.nth_child(i)).nth_child(0)))->getValueWstring();
            auto st = states.find(nextState);
            if (st == states.end()) throw runtime_error(" state " + converter.to_bytes(nextState) + " not declared");
            auto stateFinal = std::find(finals.begin(), finals.end(), nextState);

            // Copy to mantain consistency
            map<wstring, Data*> copy_global;
            for (auto it = global.begin(); it != global.end(); ++it) {
                copy_global[it->first] = new Data(*(it->second));
            }

            if (token == L"TOKEN ID" && m_atn.find(node->getValueWstring()) != m_atn.end()) { // expr is an atn
                atLeastOne = true;
                vector<Atn::OutputInternal> output = executeAtn(node->getValueWstring(), copy_global, in, init, act);
                actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
                checkOutput(output);
                for (int j = 0; j < output.size(); ++j) {
                    // Copy to mantain consistency
                    map<wstring, Data*> copy_global2;
                    for (auto itAux = output[j].global.begin(); itAux != output[j].global.end(); ++itAux) {
                        copy_global2[itAux->first] = new Data(*(itAux->second));
                    }
                    vector<Atn::OutputInternal> newOutput = executeState(in, *(st->second), output[j].init, output[j].final, copy_global2, finals, states, stateFinal != finals.end());
                    actualOutput.insert( actualOutput.end(), newOutput.begin(), newOutput.end() );
                    checkOutput(actualOutput);
                }
            }
            else { // expr is not an atn
                bool b = token == L"TOKEN STRING" && node->getValueWstring() == L"NULL";
                int increment = b ? 0 : 1;
                if (!b) {
                    // Copy to mantain consistency, but m_stack has only one lvl
                    std::stack< std::map<std::wstring, Data*> > copy_stack;
                    std::map<std::wstring, Data*> copy_map;
                    for (auto it = m_stack.top().begin(); it != m_stack.top().end(); ++it) {
                        copy_map[it->first] = new Data(*(it->second));
                    }
                    copy_stack.push(copy_map);

                    Data* value;
                    if (token == L"AND") {
                        ASTN* tkn = *((it.nth_child(0)).begin());
                        if (tkn->getToken() == L"TOKEN STRING" && tkn->getValueWstring() == L"NULL") {
                            value = evaluateExpression(it.nth_child(1), copy_global, copy_stack, in, act);
                            increment = 0;
                        }
                        else {
                            value = evaluateExpression(it, copy_global, copy_stack, in, act);
                        }
                    }
                    else {
                        value = evaluateExpression(it, copy_global, copy_stack, in, act);
                    }
                    checkBool(value);
                    b = value->getBoolValue();
                }

                if (b) {
                    atLeastOne = true;
                    vector<Atn::OutputInternal> output = executeState(in, *(st->second), init, act + increment, copy_global, finals, states, stateFinal != finals.end());
                    actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
                    checkOutput(actualOutput);
                }
            }
        }
        if (!atLeastOne && !final) {
            // Copy to mantain consistency
            map<wstring, Data*> copy_global;
            for (auto it = global.begin(); it != global.end(); ++it) {
                copy_global[it->first] = new Data(*(it->second));
            }
            vector<Atn::OutputInternal> output = executeAtn(L"main", copy_global, in, act + 1, act + 1);
            actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
            checkOutput(actualOutput);
        }
    }
    return actualOutput;
}

Data* Atn::executeFunction(std::wstring funcname, const freeling::tree<ASTN*>::const_iterator& args, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input) {
    // Get the AST of the function
    auto fun = m_func.find(funcname);
    if (fun == m_func.end()) throw runtime_error(" function " + converter.to_bytes(funcname) + " not declared");
    auto func = (fun->second)->begin();

    // Create the activation record in memory
    m_stack.push( listArguments(func, args, global, m_stack, in, input) );

    // Execute the instructions
    Data* result = executeListInstructions (func.nth_child(1).begin(), global, m_stack, in, false);
    if (result == nullptr) result = new Data();

    // Destroy the activation record
    m_stack.pop();

    return result;
}

std::map<std::wstring, Data*> Atn::listArguments(const freeling::tree<ASTN*>::const_iterator& AstF, const freeling::tree<ASTN*>::const_iterator& args, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input) {
    auto pars = AstF.nth_child(0);   // Parameters of the function
        
    // Create the map of parametres
    map<wstring, Data*> Params;
    int n = pars.num_children();

    // Check that the number of parameters is the same
    int nargs = args.num_children();
    if (n != nargs) {
        throw runtime_error("Incorrect number of parameters calling function: " + converter.to_bytes((*AstF)->getValueWstring()));
    }

    // Checks the compatibility of the parameters passed by
    // reference and calculates the values and references of
    // the parameters.
    for (int i = 0; i < n; ++i) {
        auto p_it = pars.nth_child(i); // Parameters of the callee
        auto a_it = args.nth_child(i); // Arguments passed by the caller
        ASTN p = *(*p_it);
        ASTN a = *(*a_it);
        if (p.getToken() == L"VALUE") {
            // Pass by value: evaluate the expression
            Params[p.getValueWstring()] = evaluateExpression(a_it, global, m_stack, in, input);
        } else { // p.getToken() == L"ANDPERSAND"
            // Pass by reference: check that it is a variable, array or object
            if (a.getValueWstring() == L"$") throw runtime_error("The variable $ has to pass by value");
            Params[p.getValueWstring()] = getAccesData(a, a_it, global, m_stack, in, -2);
        }
    }

    return Params;
}

Data* Atn::executeListInstructions(const freeling::tree<ASTN*>::const_iterator& t, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, bool final) {
    for (int i = 0; i < t.num_children(); ++i) {
        Data* result = executeInstruction (t.nth_child(i), global, m_stack, in, final);
        if (result != nullptr) return result;
    }
    return nullptr;
}

Data* Atn::executeInstruction(const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, bool final) {
    ASTN node = *(*it);
    wstring type = node.getToken();

    if (type == L"ASSIGMENT") {
        ASTN token = *(*(it.nth_child(0)));
        Data* value2 = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
        if (token.getToken() == L"TOKEN ID") {
            wstring nameValue = token.getValueWstring();
            if (nameValue == L"$") throw runtime_error("The variable $ it's reserved for the input");
            else if (!final && nameValue == L"@") throw runtime_error("The variable @ it's reserved for the output in final states");
            if (global.find(nameValue) != global.end()) {
                global[nameValue]->setDataValue(value2);
            }
            else if (m_stack.top().find(nameValue) != m_stack.top().end()) {
                m_stack.top()[nameValue]->setDataValue(value2);
            }
            else {
                m_stack.top()[nameValue] = value2;
            }
        }
        else if (token.getToken() == L"ARRAY ACCES") {
            auto itAux = it.nth_child(0).nth_child(0);
            Data* d = getAccesData(*(*(itAux)), itAux, global, m_stack, in, -2);
            if (d->isArray() || d->isMap()) {
                Data* pos = evaluateExpression(it.nth_child(0).nth_child(1), global, m_stack, in, -1);
                if (pos->isInt() && d->isArray()) {
                    d->addArrayValue(pos->getIntValue(), value2);
                }
                else if (pos->isWstring() && d->isMap()) {
                    d->addMapValue(pos->getWstringValue(), value2);
                }
                else {
                    throw runtime_error("Index of the array have to be int");
                }
            }
            else throw runtime_error("Acces error: Invalid object");
        }
        else if (token.getToken() == L"OBJECT ACCES") {
            auto itAux = it.nth_child(0).nth_child(0);
            Data* d = getAccesData(*(*(itAux)), itAux, global, m_stack, in, -2);
            if (d->isMap()) {
                ASTN pos = *(*(it.nth_child(0).nth_child(1)));
                d->addMapValue(pos.getValueWstring(), value2);
            }
            else throw runtime_error("Acces error: Invalid object");
        }
        else throw runtime_error("can not assign");
        return nullptr;
    }
    else if (type == L"DPLUS" or type == L"DPLUSR") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        Data* value = getAccesData(t, it.nth_child(0), global, m_stack, in, -2);
        checkNumeric(value);
        if (value->isInt()) {
            value->setIntValue(value->getIntValue() + 1);
        }
        else if (value->isDouble()) {
            value->setDoubleValue(value->getDoubleValue() + 1.0);
        }
        else assert(false);
        return nullptr;
    }
    else if (type == L"DMINUS" or type == L"DMINUSR") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        Data* value = getAccesData(t, it.nth_child(0), global, m_stack, in, -2);
        checkNumeric(value);
        if (value->isInt()) {
            value->setIntValue(value->getIntValue() - 1);
        }
        else if (value->isDouble()) {
            value->setDoubleValue(value->getDoubleValue() - 1.0);
        }
        else assert(false);
        return nullptr;   
    }
    else if (type == L"IF ELSE IF ELSE") {
        assert(it.num_children() == 4);
        Data* value = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        checkBool(value);
        if (value->getBoolValue()) return executeListInstructions(it.nth_child(1), global, m_stack, in, final);
        freeling::tree<ASTN*>::const_iterator it2 = it.nth_child(2);
        for (int i = 0; i < it2.num_children(); i = i + 2) {
            value = evaluateExpression(it2.nth_child(i), global, m_stack, in, -1);
            checkBool(value);
            if (value->getBoolValue()) return executeListInstructions(it2.nth_child(i + 1), global, m_stack, in, final);
        }
        if (it.nth_child(3).num_children() > 0) return executeListInstructions(it.nth_child(3).nth_child(0), global, m_stack, in, final);
        else return nullptr;
    }
    else if (type == L"WHILE") {
        assert(it.num_children() == 2);
        while (true) {
            Data* value = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
            checkBool(value);
            if (!value->getBoolValue()) return nullptr;
            Data* result = executeListInstructions(it.nth_child(1), global, m_stack, in, final);
            if (result != nullptr) return result;
        }
    }
    else if (type == L"DO WHILE") {
        assert(it.num_children() == 2);
        while (true) {
            Data* result = executeListInstructions(it.nth_child(0), global, m_stack, in, final);
            if (result != nullptr) return result;
            Data* value = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
            checkBool(value);
            if (!value->getBoolValue()) return nullptr;
        }
    }
    else if (type == L"FOR") {
        assert(it.num_children() == 4);
        if (it.nth_child(0).num_children() > 0) executeInstruction(it.nth_child(0), global, m_stack, in, final);
        while (true) {
            Data* value = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
            checkBool(value);
            if (!value->getBoolValue()) return nullptr;
            Data* result = executeListInstructions(it.nth_child(3), global, m_stack, in, final);
            if (result != nullptr) return result;
            if (it.nth_child(2).num_children() > 0) executeInstruction(it.nth_child(2), global, m_stack, in, final);
        }
    }
    else if (type == L"RETURN") {
        if (it.num_children() != 0) {
            return evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        }
        return new Data();
    }
    else if (type == L"PRINT") {
        assert(it.num_children() == 1);
        auto list = it.nth_child(0);
        for (auto i = 0; i != list.num_children(); ++i) {
            if ((*(list.nth_child(i)))->getToken() == L"ENDL") {
                printf("\n");
            } 
            else {
                wstring out = evaluateExpression(list.nth_child(i), global, m_stack, in, -1)->toString();
                printOutput(converter.to_bytes(out));
            }
        }
        return nullptr;
    }
    else if (type == L"FUNCALL") {
        executeFunction(node.getValueWstring(), it.nth_child(0), global, m_stack, in, -1);
        return nullptr;
    }
    else if (type == L"LOCAL FUNCTION") {
        executeLocalFunction(node, it, global, m_stack, in, -2);
        return nullptr;
    }
    else assert(false);
}

Data* Atn::evaluateExpression(const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input) {
    Data *value1, *value2;
    ASTN node = *(*it);
    wstring type = node.getToken();

    // Atoms
    if (type == L"TOKEN BOOL") {
        value1 = new Data(node.getValueBool());
    }
    else if (type == L"TOKEN INT") {
        value1 = new Data(node.getValueInt());
    }
    else if (type == L"TOKEN DOUBLE") {
        value1 = new Data(node.getValueDouble());
    }
    else if (type == L"TOKEN STRING") {
        value1 = new Data(node.getValueWstring());
    }
    else if (type == L"TOKEN OBJECT") {
        auto list = it.nth_child(0);
        map<wstring, Data*> m;
        for (int i = 0; i < list.num_children(); ++i) {
            auto elem = list.nth_child(i);
            m[(*elem)->getValueWstring()] = evaluateExpression(elem.nth_child(0), global, m_stack, in, input);
        }
        value1 = new Data(m);
    }
    else if (type == L"TOKEN ARRAY") {
        auto list = it.nth_child(0);
        vector<Data*> v(list.num_children());
        for (int i = 0; i < v.size(); ++i) {
            v[i] = evaluateExpression(list.nth_child(i), global, m_stack, in, input);
        }
        value1 = new Data(v);
    }
    else if (type == L"TOKEN ID" || type == L"ARRAY ACCES" || type == L"OBJECT ACCES") {
        value1 = new Data(getAccesData(node, it, global, m_stack, in, input));
    }
    else if (type == L"FUNCALL") {
        value1 = executeFunction(node.getValueWstring(), it.nth_child(0), global, m_stack, in, input);
        if (value1 == nullptr or value1->isVoid()) {
            throw runtime_error("function " + converter.to_bytes(node.getValueWstring()) + " expected to return a value");
        }
    }
    // Array & Object operators
    else if (type == L"LOCAL FUNCTION") {
        value1 = executeLocalFunction(node, it, global, m_stack, in, input);
    }
    // Unary operators
    else if (type == L"NOT") {
        assert(it.num_children() == 1);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        checkBool(value1);
        value1->setBoolValue(!value1->getBoolValue());
    }
    else if (type == L"NEG") {
        assert(it.num_children() == 1);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        checkNumeric(value1);
        if (value1->isInt()) {
            value1->setIntValue(-value1->getIntValue());
        }
        else if (value1->isDouble()) {
            value1->setDoubleValue(-value1->getDoubleValue());
        }
        else assert(false);
    } 
    else if (type == L"POS") {
        assert(it.num_children() == 1);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        checkNumeric(value1);
    } 
    else if (type == L"DPLUS") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        value1 = getAccesData(t, it.nth_child(0), global, m_stack, in, -2);
        checkNumeric(value1);
        if (value1->isInt()) {
            value1->setIntValue(value1->getIntValue() + 1);
        }
        else if (value1->isDouble()) {
            value1->setDoubleValue(value1->getDoubleValue() + 1.0);
        }
        else assert(false);
    } 
    else if (type == L"DPLUSR") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        value2 = getAccesData(t, it.nth_child(0), global, m_stack, in, -2);
        checkNumeric(value2);
        value1 = new Data(value2);
        if (value2->isInt()) {
            value2->setIntValue(value2->getIntValue() + 1);
        }
        else if (value2->isDouble()) {
            value2->setDoubleValue(value2->getDoubleValue() + 1.0);
        }
        else assert(false);
    }
    else if (type == L"DMINUS") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        value1 =getAccesData(t, it.nth_child(0), global, m_stack, in, -2);
        checkNumeric(value1);
        if (value1->isInt()) {
            value1->setIntValue(value1->getIntValue() - 1);
        }
        else if (value1->isDouble()) {
            value1->setDoubleValue(value1->getDoubleValue() - 1.0);
        }
        else assert(false);
    }
    else if (type == L"DMINUSR") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        value2 = getAccesData(t, it.nth_child(0), global, m_stack, in, -2);
        checkNumeric(value2);
        value1 = new Data(value2);
        if (value2->isInt()) {
            value2->setIntValue(value2->getIntValue() - 1);
        }
        else if (value2->isDouble()) {
            value2->setDoubleValue(value2->getDoubleValue() - 1.0);
        }
        else assert(false);
    } 
    // Relational operators
    else if (type == L"EQUAL" || type == L"NOT_EQUAL" || type == L"LT" || type == L"LE" || type == L"GT" || type == L"GE") {
        assert(it.num_children() == 2);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, input);
        value2 = evaluateExpression(it.nth_child(1), global, m_stack, in, input);
        value1 = value1->evaluateRelational(type, value2);
    }
    // Arithmetic operators
    else if (type == L"PLUS" || type == L"MINUS" || type == L"MULT" || type == L"DIV" || type == L"MOD") {
        assert(it.num_children() == 2);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, input);
        value2 = evaluateExpression(it.nth_child(1), global, m_stack, in, input);
        checkNumeric(value1); checkNumeric(value2);
        value1->evaluateArithmetic(type, value2);
    }
    // Boolean operators
    else if (type == L"AND" || type == L"OR") {
        assert(it.num_children() == 2);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, input);
        checkBool(value1);
        value1 = evaluateBool(type, value1, it.nth_child(1), global, m_stack, in, input);
    }
    else assert(false);
    return value1;
}

Data* Atn::getAccesData(const ASTN& t, const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input) {
    Data* v = nullptr;
    if (t.getToken() == L"TOKEN ID") {
        if (t.getValueWstring() == L"$") {
            if (input < 0) throw runtime_error("The variable $ it's reserved for the input");
            v = new Data(in[input]);
        }
        else {
            if (global.find(t.getValueWstring()) != global.end()) {
                v = global[t.getValueWstring()];
            }
            else if (m_stack.top().find(t.getValueWstring()) != m_stack.top().end()) {
                v = m_stack.top()[t.getValueWstring()];
            }
            else throw runtime_error("Can not find the variable " + converter.to_bytes(t.getValueWstring()));
        }
    }
    else if (t.getToken() == L"ARRAY ACCES") {
        ASTN id = *(*(it.nth_child(0)));
        Data* d = getAccesData(id, it.nth_child(0), global, m_stack, in, input);
        if (d->isArray() || d->isMap()) {
            Data* pos = evaluateExpression(it.nth_child(1), global, m_stack, in, input);
            if (pos->isInt()) {
                if (d->isArray()) v = d->getArrayValue(pos->getIntValue());
                else v = d->getMapValue(pos->getIntValue());
            }
            else if (pos->isWstring()) {
                v = d->getMapValue(pos->getWstringValue());
            }
            else {
                throw runtime_error("Index of the array have to be int");
            }
        }
        else if (input == -1 && d->isWstring()) { // String Acces
            Data* pos = evaluateExpression(it.nth_child(1), global, m_stack, in, input);
            if (pos->isInt()) {
                v = new Data(d->getWstringValue(pos->getIntValue()));
            }
            else {
                throw runtime_error("Index of the string have to be int");
            }
        }
        else throw runtime_error("Acces error: Invalid object");
    }
    else if (t.getToken() == L"OBJECT ACCES") {
        ASTN id = *(*(it.nth_child(0)));
        Data* d = getAccesData(id, it.nth_child(0), global, m_stack, in, input);
        if (d->isMap()) {
            ASTN pos = *(*(it.nth_child(1)));
            v = d->getMapValue(pos.getValueWstring());
        }
        else throw runtime_error("Acces error: Invalid object");
    }
    return v;
}

Data* Atn::executeLocalFunction(const ASTN& node, const freeling::tree<ASTN*>::const_iterator& it, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input) {
    Data* value = nullptr;
    ASTN* nodeId = *(it.nth_child(0));
    Data* d = getAccesData(*nodeId, it.nth_child(0), global, m_stack, in, input);
    if (node.getValueWstring() == L"size") {
        if (it.nth_child(1).num_children() > 0) throw runtime_error("Function size has 0 parameters");
        else if (d->isArray()) value = new Data(d->getSizeArray());
        else if (d->isMap()) value = new Data(d->getSizeMap());
        else if (d->isWstring()) value = new Data(d->getSizeWstring());
        else throw runtime_error("Function size only viable with string, array or map");
    }
    else if (node.getValueWstring() == L"push_back") {
        if (it.nth_child(1).num_children() != 1) throw runtime_error("Function push_back has 1 parameter");
        else if (d->isArray()) {
            auto param = it.nth_child(1).nth_child(0);
            Data* dataP = evaluateExpression(param, global, m_stack, in, -1);
            d->addArrayValue(dataP);
            value = new Data(true);
        }
        else throw runtime_error("Function push_back only viable with array");
    }
    else if (node.getValueWstring() == L"add") {
        if (it.nth_child(1).num_children() != 2) throw runtime_error("Function remove has 2 parameter");
        else if (d->isArray()) {
            auto param1 = it.nth_child(1).nth_child(0);
            Data* dataP1 = evaluateExpression(param1, global, m_stack, in, -1);
            if (dataP1->isInt()) {
                auto param2 = it.nth_child(1).nth_child(1);
                Data* dataP2 = evaluateExpression(param2, global, m_stack, in, -1);
                d->addArrayValue(dataP1->getIntValue(), dataP2);
                value = new Data(true);
            }
            else throw runtime_error("The first parameter of add has to be int when the object is an array");
        }
        else if (d->isMap()) {
            auto param1 = it.nth_child(1).nth_child(0);
            Data* dataP1 = evaluateExpression(param1, global, m_stack, in, -1);
            if (dataP1->isWstring()) {
                auto param2 = it.nth_child(1).nth_child(1);
                Data* dataP2 = evaluateExpression(param2, global, m_stack, in, -1);
                d->addMapValue(dataP1->getWstringValue(), dataP2);
                value = new Data(true);
            }
            else throw runtime_error("The first parameter of remove has to be string when the object is an map");
        }
        else throw runtime_error("Function add only viable with array or map");
    }
    else if (node.getValueWstring() == L"remove") {
        if (it.nth_child(1).num_children() != 1) throw runtime_error("Function remove has 1 parameter");
        else if (d->isArray()) {
            auto param = it.nth_child(1).nth_child(0);
            Data* dataP = evaluateExpression(param, global, m_stack, in, -1);
            if (dataP->isInt()) {
                d->deleteArrayValue(dataP->getIntValue());
                value = new Data(true);
            }
            else throw runtime_error("The parameter of remove has to be int when the object is an array");
        }
        else if (d->isMap()) {
            auto param = it.nth_child(1).nth_child(0);
            Data* dataP = evaluateExpression(param, global, m_stack, in, -1);
            if (dataP->isWstring()) {
                d->deleteMapValue(dataP->getWstringValue());
                value = new Data(true);
            }
            else throw runtime_error("The parameter of remove has to be string when the object is an map");
        }
        else throw runtime_error("Function remove only viable with array or map");
    }
    else if (node.getValueWstring() == L"indexOf") {
        if (it.nth_child(1).num_children() != 1) throw runtime_error("Function indexOf has 1 parameter");
        else if (d->isArray()) {
            auto param = it.nth_child(1).nth_child(0);
            Data* dataP = evaluateExpression(param, global, m_stack, in, -1);
            value = new Data(d->getIndexOfArray(dataP));
        }
        else throw runtime_error("Function indexOf only viable with array");
        
    }
    else if (node.getValueWstring() == L"substring") {
        int numC = it.nth_child(1).num_children();
        if (numC == 0 || numC > 2) throw runtime_error("Function substring has 1 or 2 parameters");
        else if (d->isWstring()) {
            auto param1 = it.nth_child(1).nth_child(0);
            Data* dataP1 = evaluateExpression(param1, global, m_stack, in, input);
            if (!dataP1->isInt()) throw runtime_error("Parameters of function substring have to be ints");
            if (numC == 1) {
                value = new Data((d->getWstringValue()).substr(dataP1->getIntValue()));
            }
            else {
                auto param2 = it.nth_child(1).nth_child(1);
                Data* dataP2 = evaluateExpression(param2, global, m_stack, in, input);
                if (!dataP2->isInt()) throw runtime_error("Parameters of function substring have to be ints");
                value = new Data((d->getWstringValue()).substr(dataP1->getIntValue(), dataP2->getIntValue()));
            }
        }
        else throw runtime_error("Function substring only viable with string");
    }
    else if (node.getValueWstring() == L"contain") {
        if (it.nth_child(1).num_children() != 1) throw runtime_error("Function contain has 1 parameter");
        else if (d->isMap()) {
            auto param = it.nth_child(1).nth_child(0);
            Data* dataP = evaluateExpression(param, global, m_stack, in, input);
            if (!dataP->isWstring()) throw runtime_error("Parameter of function contain has to be string (the key)");
            value = new Data(d->getContainMap(dataP->getWstringValue()));
        }
        else throw runtime_error("Function contain only viable with map");
    }
    else if (node.getValueWstring() == L"toString") {
        if (it.nth_child(1).num_children() > 0) throw runtime_error("Function size has 0 parameters");
        else if (d->isInt()) value = new Data(d->toString());
        else if (d->isDouble()) value = new Data(d->toString());
        else throw runtime_error("Function toString only viable with int or double");
    }
    else if (node.getValueWstring() == L"toInt") {
        if (it.nth_child(1).num_children() > 0) throw runtime_error("Function size has 0 parameters");
        else if (d->isWstring()) value = new Data(d->toInt());
        else if (d->isDouble()) value = new Data(d->toInt());
        else throw runtime_error("Function toInt only viable with string or double");
    }
    else if (node.getValueWstring() == L"toDouble") {
        if (it.nth_child(1).num_children() > 0) throw runtime_error("Function size has 0 parameters");
        else if (d->isInt()) value = new Data(d->toDouble());
        else if (d->isWstring()) value = new Data(d->toDouble());
        else throw runtime_error("Function toDouble only viable with int or string");
    }
    /* INSERT HERE THE NEWS LOCAL FUNCTIONS, JUST LIKE THE OTHERS */
    else throw runtime_error("function " + converter.to_bytes(node.getValueWstring()) + " indefined");
    return value;
}

Data* Atn::evaluateBool(std::wstring type, Data* v, const freeling::tree<ASTN*>::const_iterator& t, std::map<std::wstring, Data*>& global, std::stack< std::map<std::wstring, Data*> >& m_stack, const std::vector<std::wstring>& in, int input) {
    if (type == L"AND") {
        if (!v->getBoolValue()) return v;
    }
    else if (type == L"OR") {
        if (v->getBoolValue()) return v;
    }
    else assert(false);

    v = evaluateExpression(t, global, m_stack, in, input);
    checkBool(v);
    return v;
}

void Atn::checkBool(const Data* b) const {
    if (!b->isBool()) {
        throw runtime_error("expecting Boolean expression");
    }
}

void Atn::checkNumeric(const Data* b) const {
    if (!b->isInt() && !b->isDouble()) {
        throw runtime_error("expecting numerical expression");
    }
}

void Atn::printOutput(std::string s) const {
    int i = s.find("\\");
    while (i < s.size()) {
        printf("%s\n", s.substr(0, i).c_str());
        if (s.at(i + 1) == 'a') {
            printf("\a");
        }
        else if (s.at(i + 1) == 'b') {
            printf("\b");
        }
        else if (s.at(i + 1) == 'f') {
            printf("\f");
        }
        else if (s.at(i + 1) == 'n') {
            printf("\n");
        }
        else if (s.at(i + 1) == 'r') {
            printf("\r");
        }
        else if (s.at(i + 1) == 't') {
            printf("\t");
        }
        else if (s.at(i + 1) == 'v') {
            printf("\v");
        }
        else {
            printf("%c", s.at(i + 1));
        }
        s = s.substr(i + 2);
        i = s.find("\\");
    }
    printf("%s", s.c_str());
}

bool Atn::compareOutput(Atn::OutputInternal a, Atn::OutputInternal b) {
    if ((a.final - a.init) < (b.final - b.init)) return true;
    else if ((a.final - a.init) == (b.final - b.init) && a.info < b.info) return true;
    else return false;
}

bool Atn::equalsOutput(Atn::OutputInternal a, Atn::OutputInternal b) {
    return (a.init == b.init) && (a.final == b.final) && (a.info == b.info);
}

void Atn::checkOutput(std::vector<Atn::OutputInternal>& v) const {
    sort (v.begin(), v.end(), compareOutput);
    v.erase( unique( v.begin(), v.end(), equalsOutput ), v.end() );
}
