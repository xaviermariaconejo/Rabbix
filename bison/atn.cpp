#include "atn.h"

#include <fstream>
#include <sstream>

using namespace std;
using namespace ATN;

Atn::Atn() :
    m_input(),
    m_output(),
    m_func(),
    m_global(),
    m_stack(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_column(0),
    m_row(1),
    m_location(0)
{ }

Atn::Atn(wstring name) :
    m_input(),
    m_output(),
    m_func(),
    m_global(),
    m_stack(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_column(0),
    m_row(1),
    m_location(0)
{

cout << "YEAH" << endl;

    ifstream file( converter.to_bytes(name) );
    if (file)
    {
        cout << "INSIDE IF" << endl;
        stringstream buffer;

        buffer << file.rdbuf();

        file.close();

        switchInputStream(&buffer);

        // operations on the buffer...
    }
cout << "OUT IF" << endl;
    parse();

    cout << "ALL CORRECT" << endl;
}

vector<Atn::Output> Atn::run(const vector<wstring>& in) {
    m_input = in;
    // executeAtn(L"main");
    return m_output;
}

Atn::~Atn() {
    clear();
}

void Atn::clear() {
    m_column = m_location = 0;
    m_row = 1;
    m_func.clear();
    m_global.clear();
    m_input.clear();
    m_output.clear();
    while (!m_stack.empty()) {
        m_stack.top().clear();
        m_stack.pop();
    }
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

int Atn::parse() {
    m_column = m_location = 0;
    m_row = 1;
    return m_parser.parse();
}

void Atn::switchInputStream(istream *is) {
    m_scanner.switch_streams(is, NULL);
    m_func.clear();    
    m_global.clear();    
}

stringstream Atn::ASTPrint(const freeling::tree<ASTN*>& t, string tab) {
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

stringstream Atn::ASTPrint(const ATNN& node, string tab) {
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



// void Atn::executeAtn(wstring atnname) {
//     // Get the data of the ATN
//     auto atn_id = m_func.find(atnname);
//     if (atn_id == m_func.end()) throw runtime_error(" atn " + converter.to_bytes(atnname) + " not declared");
//     ATNN atn = *(((atn_id->second)->begin())->atn);

//     // Execute all the initial states at the start of the input
//     map<wstring, tree<ATNN::Node>*> states = atn.getStates();
//     map<wstring, tree<ATNN::Node>*> finals = atn.getFinals();
//     vector<wstring> list = atn.getInitials();
//     map<wstring, Data*> globals(m_global);
//     for (int i = 0; i < list.size(); ++i) {
//         wstring stateInit = list[i];
//         auto st = states.find(stateInit);
//         if (st == states.end()) throw runtime_error(" state " + converter.to_bytes(stateInit) + " not declared");
//         auto stateFinal = finals.find(stateInit);
//         map<wstring, Data*> globalsCopy(globals);
//         executeState(*(st->second), 0, 0, globalsCopy, finals, states, stateFinal != finals.end());
//     }
// }

// void Atn::executeState(const tree<ATNN::Node>& state, int i, int j, map<wstring, Data*>& global, const map<wstring, tree<ATNN::Node>*>& finalStates, const map<wstring, tree<ATNN::Node>*>& states, bool final) {
//     // Update m_global & prepare m_stack
//     m_global = global;
//     m_stack.push( map<wstring, Data*>() );


//     // Execute the list of instructions of the state, if
//     // the state is final, push the result in the vector 
//     // of results
//     if (final) {
//         executeListInstructions(((state.nth_child(0)).nth_child(0)).begin(), true);
//         if (m_stack.top().find(L"@") != m_stack.top().end()) {
//             Data* value = m_stack.top()[L"@"];
//             if (value->isWstring()) {
//                 Output newOutput(i, j, value->getWstringValue());
//                 m_output.push_back(newOutput);
//                 i = j;
//             }
//             else throw runtime_error("The output value in @ has to be a string");
//         }
//         else throw runtime_error("A final state need a valid string as output in the variable @");
//     }
//     else executeListInstructions(((state.nth_child(0)).nth_child(0)).begin());

//     // Erase the lvl of the stack for this state
//     m_stack.pop();

//     // Check all the transitions and go to
//     // the state if the result is true
//     auto list = state.nth_child(1);
//     for (int k = 0; k < list.num_children(); ++k) {
//         Data* value = evaluateExpression((list.nth_child(k)).nth_child(1), j);
//         checkBool(value);
//         if (value->getBoolValue()) {
//             wstring nextState = ((list.nth_child(i)).nth_child(0)->astn)->getValueWstring();
//             auto st = states.find(nextState);
//             if (st == states.end()) throw runtime_error(" state " + converter.to_bytes(nextState) + " not declared");
//             auto stateFinal = finalStates.find(nextState);
//             map<wstring, Data*> globalsCopy(m_global);
//             executeState(*(st->second), i, j + 1, globalsCopy, finalStates, states, stateFinal != finalStates.end());
//         }
//     }
// }

// Data* Atn::executeFunction(wstring funcname, const freeling::tree<ATNN::Node>::const_iterator& args, int input) {
//     // Get the AST of the function
//     auto fun = m_func.find(funcname);
//     if (fun == m_func.end()) throw runtime_error(" function " + converter.to_bytes(funcname) + " not declared");
//     auto func = (fun->second)->begin();

//     // Create the activation record in memory
//     m_stack.push( listArguments(func, args, input) );

//     // Execute the instructions
//     Data* result = executeListInstructions (func.nth_child(1).begin());
//     if (result == nullptr) result = new Data();

//     // Destroy the activation record
//     m_stack.pop();

//     return result;
// }

// map<wstring, Data*> Atn::listArguments(const freeling::tree<ATNN::Node>::const_iterator& AstF, const freeling::tree<ATNN::Node>::const_iterator& args, int input) {
//     auto pars = AstF.nth_child(0);   // Parameters of the function
        
//     // Create the map of parametres
//     map<wstring, Data*> Params;
//     int n = pars.num_children();

//     // Check that the number of parameters is the same
//     int nargs = args.num_children();
//     if (n != nargs) {
//         throw runtime_error("Incorrect number of parameters calling function: " + converter.to_bytes((AstF->astn)->getValueWstring()));
//     }

//     // Checks the compatibility of the parameters passed by
//     // reference and calculates the values and references of
//     // the parameters.
//     for (int i = 0; i < n; ++i) {
//         auto p_it = pars.nth_child(i); // Parameters of the callee
//         auto a_it = args.nth_child(i); // Arguments passed by the caller
//         ASTN p = *(p_it->astn);
//         ASTN a = *(a_it->astn);
//         if (p.getToken() == L"VALUE") {
//             // Pass by value: evaluate the expression
//             Params[p.getValueWstring()] = evaluateExpression(a_it, input);
//         } else { // p.getToken() == L"ANDPERSAND"
//             // Pass by reference: check that it is a variable, array or object
//             if (a.getValueWstring() == L"$") throw runtime_error("The variable $ has to pass by value");
//             Params[p.getValueWstring()] = getAccesData(a, a_it);
//         }
//     }

//     return Params;
// }

// Data* Atn::executeListInstructions(const freeling::tree<ATNN::Node>::const_iterator& t, bool final) {
//     for (int i = 0; i < t.num_children(); ++i) {
//         Data* result = executeInstruction (t.nth_child(i), final);
//         if (result != nullptr) return result;
//     }
//     return nullptr;
// }

// Data* Atn::executeInstruction(const freeling::tree<ATNN::Node>::const_iterator& it, bool final) {
//     ASTN node = *(it->astn);
//     wstring type = node.getToken();

//     if (type == L"ASSIGMENT") {
//         ASTN value1 = *((it.nth_child(0))->astn);
//         wstring nameValue = value1.getValueWstring();
//         if (nameValue == L"$") throw runtime_error("The variable $ it's reserved for the input");
//         else if (!final && nameValue == L"@") throw runtime_error("The variable @ it's reserved for the output in final states");
//         Data* value2 = evaluateExpression(it.nth_child(1));
//         if (value1.getToken() == L"TOKEN ID") {
//             if (m_global.find(nameValue) != m_global.end()) {
//                 m_global[nameValue]->setDataValue(value2);
//             }
//             else if (m_stack.top().find(nameValue) != m_stack.top().end()) {
//                 m_stack.top()[nameValue]->setDataValue(value2);
//             }
//             else {
//                 m_stack.top()[nameValue] = value2;
//             }
//         }
//         else if (value1.getToken() == L"ARRAY ACCES") {
//             Data* d = nullptr;
//             if (m_global.find(nameValue) != m_global.end()) d = m_global[nameValue];
//             else if (m_stack.top().find(nameValue) != m_stack.top().end()) d = m_stack.top()[nameValue];
//             if (d == nullptr || d->isArray() || d->isMap()) {
//                 Data* pos = evaluateExpression(it.nth_child(1));
//                 if (pos->isInt() && (d == nullptr || d->isArray())) {
//                     if (d == nullptr) d = new Data(vector<Data*>());
//                     d->addArrayValue(pos->getIntValue(), value2);
//                 }
//                 else if (pos->isWstring() && (d == nullptr || d->isMap())) {
//                     if (d == nullptr) d = new Data(map<wstring, Data*>());
//                     d->addMapValue(pos->getWstringValue(), value2);
//                 }
//                 else {
//                     throw runtime_error("Index of the array have to be int");
//                 }
//             }
//             else throw runtime_error("Acces error: Invalid object");
//         }
//         else if (value1.getToken() == L"OBJECT ACCES") {
//             Data* d = nullptr;
//             if (m_global.find(nameValue) != m_global.end()) d = m_global[nameValue];
//             if (m_stack.top().find(nameValue) != m_stack.top().end()) d = m_stack.top()[nameValue];
//             else d = new Data(map<wstring, Data*>());
//             if (d->isMap()) {
//                 Data* pos = evaluateExpression(it.nth_child(1));
//                 if (pos->isWstring()) {
//                     d->addMapValue(pos->getWstringValue(), value2);
//                 }
//                 else {
//                     throw runtime_error("Key of the map have to be string");
//                 }
//             }
//             else throw runtime_error("Acces error: Invalid object");
//         }
//         else throw runtime_error("can not assign");
//         return nullptr;
//     }
//     else if (type == L"DPLUS" or type == L"DPLUSR") {
//         assert(it.num_children() == 1);
//         ASTN t = *((it.nth_child(0))->astn);
//         Data* value = getAccesData(t, it);
//         checkNumeric(value);
//         if (value->isInt()) {
//             value->setIntValue(value->getIntValue() + 1);
//         }
//         else if (value->isDouble()) {
//             value->setDoubleValue(value->getDoubleValue() + 1.0);
//         }
//         else assert(false);
//         return nullptr;
//     }
//     else if (type == L"DMINUS" or type == L"DMINUSR") {
//         assert(it.num_children() == 1);
//         ASTN t = *((it.nth_child(0))->astn);
//         Data* value = getAccesData(t, it);
//         checkNumeric(value);
//         if (value->isInt()) {
//             value->setIntValue(value->getIntValue() - 1);
//         }
//         else if (value->isDouble()) {
//             value->setDoubleValue(value->getDoubleValue() - 1.0);
//         }
//         else assert(false);
//         return nullptr;   
//     }
//     else if (type == L"IF ELSE IF ELSE") {
//         assert(it.num_children() == 4);
//         Data* value = evaluateExpression(it.nth_child(0));
//         checkBool(value);
//         if (value->getBoolValue()) return executeListInstructions(it.nth_child(1));
//         freeling::tree<ATNN::Node>::const_iterator it2 = it.nth_child(2);
//         for (int i = 0; i < it2.num_children(); i = i + 2) {
//             value = evaluateExpression(it2.nth_child(i));
//             checkBool(value);
//             if (value->getBoolValue()) return executeListInstructions(it2.nth_child(i + 1));
//         }
//         if (it.nth_child(3).num_children() > 0) return executeListInstructions(it.nth_child(3).nth_child(0));
//         else return nullptr;
//     }
//     else if (type == L"WHILE") {
//         assert(it.num_children() == 2);
//         while (true) {
//             Data* value = evaluateExpression(it.nth_child(0));
//             checkBool(value);
//             if (!value->getBoolValue()) return nullptr;
//             Data* result = executeListInstructions(it.nth_child(1));
//             if (result != nullptr) return result;
//         }
//     }
//     else if (type == L"DO WHILE") {
//         assert(it.num_children() == 2);
//         while (true) {
//             Data* result = executeListInstructions(it.nth_child(0));
//             if (result != nullptr) return result;
//             Data* value = evaluateExpression(it.nth_child(1));
//             checkBool(value);
//             if (!value->getBoolValue()) return nullptr;
//         }
//     }
//     else if (type == L"FOR") {
//         assert(it.num_children() == 4);
//         if (it.nth_child(0).num_children() > 0) executeInstruction(it.nth_child(0));
//         while (true) {
//             Data* value = evaluateExpression(it.nth_child(1));
//             checkBool(value);
//             if (!value->getBoolValue()) return nullptr;
//             Data* result = executeListInstructions(it.nth_child(3));
//             if (result != nullptr) return result;
//             if (it.nth_child(2).num_children() > 0) executeInstruction(it.nth_child(2));
//         }
//     }
//     else if (type == L"RETURN") {
//         if (it.num_children() != 0) {
//             return evaluateExpression(it.nth_child(0));
//         }
//         return new Data();
//     }
//     else if (type == L"PRINT") {
//         assert(it.num_children() == 1);
//         auto list = it.nth_child(0);
//         for (auto i = 0; i != list.num_children(); ++i) {
//             if ((list.nth_child(i)->astn)->getToken() == L"ENDL") {
//                 printf("\n");
//             } 
//             else {
//                 wstring out = evaluateExpression(list.nth_child(i))->toString();
//                 printOutput(converter.to_bytes(out));
//             }
//         }
//         return nullptr;
//     }
//     else if (type == L"FUNCALL") {
//         executeFunction(node.getValueWstring(), it.nth_child(0));
//         return nullptr;
//     }
//     else assert(false);
// }

// template<sentence,word>
// Data* Atn::evaluateExpression(const freeling::tree<ATNN::Node>::const_iterator& it, int input) {
//     Data *value1, *value2;
//     ASTN node = *(it->astn);
//     wstring type = node.getToken();
        
//     // Atoms
//     if (type == L"TOKEN BOOL") {
//         value1 = new Data(node.getValueBool());
//     }
//     else if (type == L"TOKEN INT") {
//         value1 = new Data(node.getValueInt());
//     }
//     else if (type == L"TOKEN DOUBLE") {
//         value1 = new Data(node.getValueDouble());
//     }
//     else if (type == L"TOKEN STRING") {
//         value1 = new Data(node.getValueWstring());
//     }
//     else if (type == L"TOKEN OBJECT") {
//         auto list = it.nth_child(0);
//         map<wstring, Data*> m;
//         for (int i = 0; i < list.num_children(); ++i) {
//             auto elem = list.nth_child(i);
//             m[(elem->astn)->getValueWstring()] = evaluateExpression(elem.nth_child(0));
//         }
//         value1 = new Data(m);
//     }
//     else if (type == L"TOKEN ARRAY") {
//         auto list = it.nth_child(0);
//         vector<Data*> v(list.num_children());
//         for (int i = 0; i < v.size(); ++i) {
//             v[i] = evaluateExpression(list.nth_child(i));
//         }
//         value1 = new Data(v);
//     }
//     else if (type == L"TOKEN ID" || type == L"ARRAY ACCES" || type == L"OBJECT ACCES") {
//         value1 = new Data(getAccesData(node, it, input));
//     }
//     else if (type == L"FUNCALL") {
//         value1 = executeFunction(node.getValueWstring(), it.nth_child(0), input);
//         if (value1 == nullptr or value1->isVoid()) {
//             throw runtime_error("function " + converter.to_bytes(node.getValueWstring()) + " expected to return a value");
//         }
//     }
//     // Array & Object operators
//     else if (type == L"LOCAL FUNCTION") {
//         Data* d = nullptr;
//         ASTN* nodeId = (it.nth_child(0))->astn;
//         wstring nameId = nodeId->getValueWstring();
//         if (m_global.find(nameId) != m_global.end()) {
//             d = m_global[nameId];
//         }
//         else if (m_stack.top().find(nameId) != m_stack.top().end()) {
//             d = m_stack.top()[nameId];
//         }
//         else throw runtime_error("The variable " + converter.to_bytes(nameId) + " can't be void");

//         if (node.getValueWstring() == L"size") {
//             if (d->isArray()) value1 = new Data(d->getSizeArray());
//             else if (d->isMap()) value1 = new Data(d->getSizeMap());
//             else if (d->isWstring()) value1 = new Data(d->getSizeWstring());
//             else throw runtime_error("Function size only viable with string, array or map");
//         }
//         else if (node.getValueWstring() == L"add") {
            
//         }
//         else if (node.getValueWstring() == L"remove") {
            
//         }
//         else if (node.getValueWstring() == L"indexOf") {
            
//         }
//         else if (node.getValueWstring() == L"substring") {
            
//         }
//         else if (input && node.getValueWstring() == L"lema") {

//         }//form tag
//         else throw runtime_error("function " + converter.to_bytes(node.getValueWstring()) + " indefined");
//         //TODO
//     }
//     // Unary operators
//     else if (type == L"NOT") {
//         assert(it.num_children() == 1);
//         value1 = evaluateExpression(it.nth_child(0));
//         checkBool(value1);
//         value1->setBoolValue(!value1->getBoolValue());
//     }
//     else if (type == L"NEG") {
//         assert(it.num_children() == 1);
//         value1 = evaluateExpression(it.nth_child(0));
//         checkNumeric(value1);
//         if (value1->isInt()) {
//             value1->setIntValue(-value1->getIntValue());
//         }
//         else if (value1->isDouble()) {
//             value1->setDoubleValue(-value1->getDoubleValue());
//         }
//         else assert(false);
//     } 
//     else if (type == L"POS") {
//         assert(it.num_children() == 1);
//         value1 = evaluateExpression(it.nth_child(0));
//         checkNumeric(value1);
//     } 
//     else if (type == L"DPLUS") {
//         assert(it.num_children() == 1);
//         ASTN t = *((it.nth_child(0))->astn);
//         value1 = getAccesData(t, it);
//         checkNumeric(value1);
//         if (value1->isInt()) {
//             value1->setIntValue(value1->getIntValue() + 1);
//         }
//         else if (value1->isDouble()) {
//             value1->setDoubleValue(value1->getDoubleValue() + 1.0);
//         }
//         else assert(false);
//     } 
//     else if (type == L"DPLUSR") {
//         assert(it.num_children() == 1);
//         ASTN t = *((it.nth_child(0))->astn);
//         value2 = getAccesData(t, it);
//         checkNumeric(value2);
//         value1 = new Data(value2);
//         if (value2->isInt()) {
//             value2->setIntValue(value2->getIntValue() + 1);
//         }
//         else if (value2->isDouble()) {
//             value2->setDoubleValue(value2->getDoubleValue() + 1.0);
//         }
//         else assert(false);
//     }
//     else if (type == L"DMINUS") {
//         assert(it.num_children() == 1);
//         ASTN t = *((it.nth_child(0))->astn);
//         value1 =getAccesData(t, it);
//         checkNumeric(value1);
//         if (value1->isInt()) {
//             value1->setIntValue(value1->getIntValue() - 1);
//         }
//         else if (value1->isDouble()) {
//             value1->setDoubleValue(value1->getDoubleValue() - 1.0);
//         }
//         else assert(false);
//     }
//     else if (type == L"DMINUSR") {
//         assert(it.num_children() == 1);
//         ASTN t = *((it.nth_child(0))->astn);
//         value2 = getAccesData(t, it);
//         checkNumeric(value2);
//         value1 = new Data(value2);
//         if (value2->isInt()) {
//             value2->setIntValue(value2->getIntValue() - 1);
//         }
//         else if (value2->isDouble()) {
//             value2->setDoubleValue(value2->getDoubleValue() - 1.0);
//         }
//         else assert(false);
//     } 
//     // Relational operators
//     else if (type == L"EQUAL" || type == L"NOT_EQUAL" || type == L"LT" || type == L"LE" || type == L"GT" || type == L"GE") {
//         assert(it.num_children() == 2);
//         value1 = evaluateExpression(it.nth_child(0));
//         value2 = evaluateExpression(it.nth_child(1));
//         value1 = value1->evaluateRelational(type, value2);
//     }
//     // Arithmetic operators
//     else if (type == L"PLUS" || type == L"MINUS" || type == L"MULT" || type == L"DIV" || type == L"MOD") {
//         assert(it.num_children() == 2);
//         value1 = evaluateExpression(it.nth_child(0));
//         value2 = evaluateExpression(it.nth_child(1));
//         checkNumeric(value1); checkNumeric(value2);
//         value1->evaluateArithmetic(type, value2);
//     }
//     // Boolean operators
//     else if (type == L"AND" || type == L"OR") {
//         assert(it.num_children() == 2);
//         value1 = evaluateExpression(it.nth_child(0));
//         checkBool(value1);
//         value1 = evaluateBool(type, value1, it.nth_child(1));
//     }
//     else assert(false);
//     return value1;
// }

// Data* Atn::getAccesData(const ASTN& t, const freeling::tree<ATNN::Node>::const_iterator& it, int input) {
//     Data* v = nullptr;
//     if (t.getToken() == L"TOKEN ID") {
//         if (t.getValueWstring() == L"$") {
//             if (input < 0) throw runtime_error("The variable $ it's reserved for the input");
//             v = new Data(m_input[input]);
//         }
//         else {
//             if (m_global.find(t.getValueWstring()) != m_global.end()) {
//                 v = m_global[t.getValueWstring()];
//             }
//             else if (m_stack.top().find(t.getValueWstring()) != m_stack.top().end()) {
//                 v = m_stack.top()[t.getValueWstring()];
//             }
//             else throw runtime_error("Can not find the variable " + converter.to_bytes(t.getValueWstring()));
//         }
//     }
//     else if (t.getToken() == L"ARRAY ACCES") {
//         ASTN id = *((it.nth_child(0))->astn);
//         Data* d = nullptr;
//         if (id.getValueWstring() == L"$") {
//             throw runtime_error("The variable $ it's reserved for the input");
//         }
//         if (m_global.find(id.getValueWstring()) != m_global.end()) {
//             d = m_global[id.getValueWstring()];
//         }
//         else if (m_stack.top().find(id.getValueWstring()) != m_stack.top().end()) {
//             d = m_stack.top()[id.getValueWstring()];
//         }
//         else throw runtime_error("Can not find the variable " + converter.to_bytes(t.getValueWstring()));
//         if (d->isArray() || d->isMap()) {
//             Data* pos = evaluateExpression(it.nth_child(1));
//             if (pos->isInt()) {
//                 if (d->isArray()) v = d->getArrayValue(pos->getIntValue());
//                 else v = d->getMapValue(pos->getIntValue());
//             }
//             else if (pos->isWstring()) {
//                 v = d->getMapValue(pos->getWstringValue());
//             }
//             else {
//                 throw runtime_error("Index of the array have to be int");
//             }
//         }
//         else if (input == -1 && d->isWstring()) { // String Acces
//             Data* pos = evaluateExpression(it.nth_child(1));
//             if (pos->isInt()) {
//                 v = new Data(d->getWstringValue(pos->getIntValue()));
//             }
//             else {
//                 throw runtime_error("Index of the string have to be int");
//             }
//         }
//         else throw runtime_error("Acces error: Invalid object");
//     }
//     else if (t.getToken() == L"OBJECT ACCES") {
//         ASTN id = *((it.nth_child(0))->astn);
//         Data* d = nullptr;
//         if (id.getValueWstring() == L"$") {
//             throw runtime_error("The variable $ it's reserved for the input");
//         }
//         if (m_global.find(id.getValueWstring()) != m_global.end()) {
//             d = m_global[id.getValueWstring()];
//         }
//         else if (m_stack.top().find(id.getValueWstring()) != m_stack.top().end()) {
//             d = m_stack.top()[id.getValueWstring()];
//         }
//         else throw runtime_error("Can not find the variable " + converter.to_bytes(t.getValueWstring()));
//         if (d->isMap()) {
//             Data* pos = evaluateExpression(it.nth_child(1));
//             if (pos->isWstring()) {
//                 v = d->getMapValue(pos->getWstringValue());
//             }
//             else {
//                 throw runtime_error("Key of the map have to be string");
//             }
//         }
//         else throw runtime_error("Acces error: Invalid object");
//     }
//     return v;
// }

// Data* Atn::evaluateBool(wstring type, Data* v, const freeling::tree<ATNN::Node>::const_iterator& t) {
//     if (type == L"AND") {
//         if (!v->getBoolValue()) return v;
//     }
//     else if (type == L"OR") {
//         if (v->getBoolValue()) return v;
//     }
//     else assert(false);

//     v = evaluateExpression(t);
//     checkBool(v);
//     return v;
// }

// void Atn::checkBool(const Data* b) {
//     if (!b->isBool()) {
//         throw runtime_error("expecting Boolean expression");
//     }
// }

// void Atn::checkNumeric(const Data* b) {
//     if (!b->isInt() && !b->isDouble()) {
//         throw runtime_error("expecting numerical expression");
//     }
// }

// void Atn::printOutput(string s) const {
//     int i = s.find("\\");
//     while (i < s.size()) {
//         printf("%s\n", s.substr(0, i).c_str());
//         if (s.at(i + 1) == 'a') {
//             printf("\a");
//         }
//         else if (s.at(i + 1) == 'b') {
//             printf("\b");
//         }
//         else if (s.at(i + 1) == 'f') {
//             printf("\f");
//         }
//         else if (s.at(i + 1) == 'n') {
//             printf("\n");
//         }
//         else if (s.at(i + 1) == 'r') {
//             printf("\r");
//         }
//         else if (s.at(i + 1) == 't') {
//             printf("\t");
//         }
//         else if (s.at(i + 1) == 'v') {
//             printf("\v");
//         }
//         else {
//             printf("%c", s.at(i + 1));
//         }
//         s = s.substr(i + 2);
//         i = s.find("\\");
//     }
//     printf("%s", s.c_str());
// }
