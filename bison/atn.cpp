#include "atn.h"

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
    return executeAtn(L"main", in, 0, 0);
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



std::vector<Atn::Output> Atn::executeAtn(std::wstring atnname, const std::vector<std::wstring>& in, int init, int act) {
    // Get the data of the ATN
    auto atn_id = m_atn.find(atnname);
    if (atn_id == m_atn.end()) throw runtime_error(" atn " + converter.to_bytes(atnname) + " not declared");
    ATNN atn = *(atn_id->second);

    // Inicializate output
    vector<Atn::Output> finalOutput;

    // Execute all the initial states at the start of the input
    vector<wstring> initials = atn.getInitials();
    vector<wstring> finals = atn.getFinals();
    map<wstring, tree<ASTN*>*> states = atn.getStates();
    for (int i = 0; i < initials.size(); ++i) {
        wstring stateInit = initials[i];
        auto st = states.find(stateInit);
        if (st == states.end()) throw runtime_error(" state " + converter.to_bytes(stateInit) + " not declared");
        int stateFinal = find(finals, stateInit);
        vector<Atn::Output> output = executeState(atnname, in, *(st->second), init, act, m_global, finals, states, stateFinal > -1);
        finalOutput.insert( finalOutput.end(), output.begin(), output.end() );
    }

    return finalOutput;
}

std::vector<Atn::Output> Atn::executeState(std::wstring atnname, const std::vector<std::wstring>& in, const freeling::tree<ASTN*>& state, int init, int act, std::map<std::wstring, Data*> global, const std::vector<std::wstring>& finals, const std::map<std::wstring, freeling::tree<ASTN*>*>& states, bool final) {
    vector<Atn::Output> actualOutput;
    // Inicializate de stack of the state & output
    std::stack< std::map<std::wstring, Data*> > m_stack; std::map<std::wstring, Data*> m;
    m_stack.push(m);

    // Execute the list of instructions of the state, if
    // the state is final, push the result in the vector 
    // of results
    executeListInstructions(((state.nth_child(0)).nth_child(0)).begin(), global, m_stack, in, final);
    if (final) {
        if (m_stack.top().find(L"@") != m_stack.top().end()) {
            Data* value = m_stack.top()[L"@"];
            if (value->isWstring()) {
                Output newOutput(init, act, value->getWstringValue());
                actualOutput.push_back(newOutput);
                if (act < in.size()) {
                    vector<Atn::Output> output = executeAtn(atnname, in, act, act);
                    actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
                }
            }
            else throw runtime_error("The output value in @ has to be a string");
        }
        else throw runtime_error("A final state need a valid string as output in the variable @");
    }

    // Erase the lvl of the stack for this state
    m_stack.pop();

    if (act < in.size()) {
        // Check all the transitions and go to
        // the state if the result is true
        auto list = state.nth_child(1);
        for (int i = 0; i < list.num_children(); ++i) {
            // Get the data necessary
            wstring nextState = (*((list.nth_child(i)).nth_child(0)))->getValueWstring();
            auto st = states.find(nextState);
            if (st == states.end()) throw runtime_error(" state " + converter.to_bytes(nextState) + " not declared");
            int stateFinal = find(finals, nextState);

            // Check if is an atn or a expr
            auto it = (list.nth_child(i)).nth_child(1);
            ASTN* node = *(it.begin());
            wstring token = node->getToken();
            if (token == L"TOKEN ID" && m_atn.find(node->getValueWstring()) != m_atn.end()) { // expr is an atn
                vector<Atn::Output> output = executeAtn(node->getValueWstring(), in, init, act);
                actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
                for (int j = 0; j < output.size(); ++j) {
                    vector<Atn::Output> newOutput = executeState(atnname, in, *(st->second), output[j].init, output[j].final, global, finals, states, stateFinal > -1);
                    actualOutput.insert( actualOutput.end(), newOutput.begin(), newOutput.end() );
                }
            }
            else { // expr is not an atn
                Data* value = evaluateExpression(it, global, m_stack, in, act);
                checkBool(value);
                if (value->getBoolValue()) {
                    vector<Atn::Output> output = executeState(atnname, in, *(st->second), init, act + 1, global, finals, states, stateFinal > -1);
                    actualOutput.insert( actualOutput.end(), output.begin(), output.end() );
                }
            }
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
        ASTN value1 = *(*(it.nth_child(0)));
        wstring nameValue = value1.getValueWstring();
        if (nameValue == L"$") throw runtime_error("The variable $ it's reserved for the input");
        else if (!final && nameValue == L"@") throw runtime_error("The variable @ it's reserved for the output in final states");
        Data* value2 = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
        if (value1.getToken() == L"TOKEN ID") {
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
        else if (value1.getToken() == L"ARRAY ACCES") {
            Data* d = nullptr;
            if (global.find(nameValue) != global.end()) d = global[nameValue];
            else if (m_stack.top().find(nameValue) != m_stack.top().end()) d = m_stack.top()[nameValue];
            if (d == nullptr || d->isArray() || d->isMap()) {
                Data* pos = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
                if (pos->isInt() && (d == nullptr || d->isArray())) {
                    if (d == nullptr) d = new Data(vector<Data*>());
                    d->addArrayValue(pos->getIntValue(), value2);
                }
                else if (pos->isWstring() && (d == nullptr || d->isMap())) {
                    if (d == nullptr) d = new Data(map<wstring, Data*>());
                    d->addMapValue(pos->getWstringValue(), value2);
                }
                else {
                    throw runtime_error("Index of the array have to be int");
                }
            }
            else throw runtime_error("Acces error: Invalid object");
        }
        else if (value1.getToken() == L"OBJECT ACCES") {
            Data* d = nullptr;
            if (global.find(nameValue) != global.end()) d = global[nameValue];
            if (m_stack.top().find(nameValue) != m_stack.top().end()) d = m_stack.top()[nameValue];
            else d = new Data(map<wstring, Data*>());
            if (d->isMap()) {
                Data* pos = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
                if (pos->isWstring()) {
                    d->addMapValue(pos->getWstringValue(), value2);
                }
                else {
                    throw runtime_error("Key of the map have to be string");
                }
            }
            else throw runtime_error("Acces error: Invalid object");
        }
        else throw runtime_error("can not assign");
        return nullptr;
    }
    else if (type == L"DPLUS" or type == L"DPLUSR") {
        assert(it.num_children() == 1);
        ASTN t = *(*(it.nth_child(0)));
        Data* value = getAccesData(t, it, global, m_stack, in, -2);
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
        Data* value = getAccesData(t, it, global, m_stack, in, -2);
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
        if (value->getBoolValue()) return executeListInstructions(it.nth_child(1), global, m_stack, in, false);
        freeling::tree<ASTN*>::const_iterator it2 = it.nth_child(2);
        for (int i = 0; i < it2.num_children(); i = i + 2) {
            value = evaluateExpression(it2.nth_child(i), global, m_stack, in, -1);
            checkBool(value);
            if (value->getBoolValue()) return executeListInstructions(it2.nth_child(i + 1), global, m_stack, in, false);
        }
        if (it.nth_child(3).num_children() > 0) return executeListInstructions(it.nth_child(3).nth_child(0), global, m_stack, in, false);
        else return nullptr;
    }
    else if (type == L"WHILE") {
        assert(it.num_children() == 2);
        while (true) {
            Data* value = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
            checkBool(value);
            if (!value->getBoolValue()) return nullptr;
            Data* result = executeListInstructions(it.nth_child(1), global, m_stack, in, false);
            if (result != nullptr) return result;
        }
    }
    else if (type == L"DO WHILE") {
        assert(it.num_children() == 2);
        while (true) {
            Data* result = executeListInstructions(it.nth_child(0), global, m_stack, in, false);
            if (result != nullptr) return result;
            Data* value = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
            checkBool(value);
            if (!value->getBoolValue()) return nullptr;
        }
    }
    else if (type == L"FOR") {
        assert(it.num_children() == 4);
        if (it.nth_child(0).num_children() > 0) executeInstruction(it.nth_child(0), global, m_stack, in, false);
        while (true) {
            Data* value = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
            checkBool(value);
            if (!value->getBoolValue()) return nullptr;
            Data* result = executeListInstructions(it.nth_child(3), global, m_stack, in, false);
            if (result != nullptr) return result;
            if (it.nth_child(2).num_children() > 0) executeInstruction(it.nth_child(2), global, m_stack, in, false);
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
            m[(*elem)->getValueWstring()] = evaluateExpression(elem.nth_child(0), global, m_stack, in, -1);
        }
        value1 = new Data(m);
    }
    else if (type == L"TOKEN ARRAY") {
        auto list = it.nth_child(0);
        vector<Data*> v(list.num_children());
        for (int i = 0; i < v.size(); ++i) {
            v[i] = evaluateExpression(list.nth_child(i), global, m_stack, in, -1);
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
        Data* d = nullptr;
        ASTN* nodeId = *(it.nth_child(0));
        wstring nameId = nodeId->getValueWstring();
        if (global.find(nameId) != global.end()) {
            d = global[nameId];
        }
        else if (m_stack.top().find(nameId) != m_stack.top().end()) {
            d = m_stack.top()[nameId];
        }
        else throw runtime_error("The variable " + converter.to_bytes(nameId) + " can't be void");

        if (node.getValueWstring() == L"size") {
            if (d->isArray()) value1 = new Data(d->getSizeArray());
            else if (d->isMap()) value1 = new Data(d->getSizeMap());
            else if (d->isWstring()) value1 = new Data(d->getSizeWstring());
            else throw runtime_error("Function size only viable with string, array or map");
        }
        else if (node.getValueWstring() == L"add") {
            
        }
        else if (node.getValueWstring() == L"remove") {
            
        }
        else if (node.getValueWstring() == L"indexOf") {
            
        }
        else if (node.getValueWstring() == L"substring") {
            
        }
        else if (input && node.getValueWstring() == L"lema") {

        }//form tag
        else throw runtime_error("function " + converter.to_bytes(node.getValueWstring()) + " indefined");
        //TODO
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
        value1 = getAccesData(t, it, global, m_stack, in, -2);
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
        value2 = getAccesData(t, it, global, m_stack, in, -2);
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
        value1 =getAccesData(t, it, global, m_stack, in, -2);
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
        value2 = getAccesData(t, it, global, m_stack, in, -2);
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
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        value2 = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
        value1 = value1->evaluateRelational(type, value2);
    }
    // Arithmetic operators
    else if (type == L"PLUS" || type == L"MINUS" || type == L"MULT" || type == L"DIV" || type == L"MOD") {
        assert(it.num_children() == 2);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
        value2 = evaluateExpression(it.nth_child(1), global, m_stack, in, -1);
        checkNumeric(value1); checkNumeric(value2);
        value1->evaluateArithmetic(type, value2);
    }
    // Boolean operators
    else if (type == L"AND" || type == L"OR") {
        assert(it.num_children() == 2);
        value1 = evaluateExpression(it.nth_child(0), global, m_stack, in, -1);
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
        Data* d = nullptr;
        if (id.getValueWstring() == L"$") {
            throw runtime_error("The variable $ it's reserved for the input");
        }
        if (global.find(id.getValueWstring()) != global.end()) {
            d = global[id.getValueWstring()];
        }
        else if (m_stack.top().find(id.getValueWstring()) != m_stack.top().end()) {
            d = m_stack.top()[id.getValueWstring()];
        }
        else throw runtime_error("Can not find the variable " + converter.to_bytes(t.getValueWstring()));
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
        Data* d = nullptr;
        if (id.getValueWstring() == L"$") {
            throw runtime_error("The variable $ it's reserved for the input");
        }
        if (global.find(id.getValueWstring()) != global.end()) {
            d = global[id.getValueWstring()];
        }
        else if (m_stack.top().find(id.getValueWstring()) != m_stack.top().end()) {
            d = m_stack.top()[id.getValueWstring()];
        }
        else throw runtime_error("Can not find the variable " + converter.to_bytes(t.getValueWstring()));
        if (d->isMap()) {
            Data* pos = evaluateExpression(it.nth_child(1), global, m_stack, in, input);
            if (pos->isWstring()) {
                v = d->getMapValue(pos->getWstringValue());
            }
            else {
                throw runtime_error("Key of the map have to be string");
            }
        }
        else throw runtime_error("Acces error: Invalid object");
    }
    return v;
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

int Atn::find(const std::vector<std::wstring>& v, std::wstring ws) const {
    int i = 0; bool b = false;
    while(i < v.size() && !b) {
        wstring abc = v[i];
        b = (v[i++] == ws);
    }
    if (!b) i = -1;
    return i;
}
