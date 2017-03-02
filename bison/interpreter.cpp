#include "interpreter.h"

#include <sstream>

using namespace std;
using namespace ATN;

Interpreter::Interpreter() :
    m_map(),
    m_stack(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_column(0),
    m_row(1),
    m_location(0)
{ }

Interpreter::~Interpreter() {
    clear();
}

int Interpreter::parse() {
    m_column = m_location = 0;
    m_row = 1;
    return m_parser.parse();
}

void Interpreter::run() {
    /*if (m_map.find(L"settings") != m_map.end())
        executeFunction (L"settings", freeling::tree<ASTN>(ASTN(L"PARAM LIST")));
    executeAtn(L"main");*/
}

void Interpreter::clear() {
    m_column = m_location = 0;
    m_row = 1;
    m_map.clear();
    while (!m_stack.empty()) {
        m_stack.top().clear();
        m_stack.pop();
    }
}

string Interpreter::str() const {
    stringstream s;
    s << "Interpreter: Printing all elements" << endl << endl;
    for (auto it = m_map.begin(); it != m_map.end(); ++it) {
        freeling::tree<ASTN> t = it->second;
        s << ASTPrint(t, "").str() << endl;
    }
    return s.str();
}

void Interpreter::switchInputStream(istream *is) {
    m_scanner.switch_streams(is, NULL);
    m_map.clear();    
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

void Interpreter::addMainElement(wstring ws, const freeling::tree<ASTN>& t)
{
    m_map[ws] = t;
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



void Interpreter::executeAtn(wstring atnName) {

}

Data Interpreter::executeFunction(wstring funcname, const freeling::tree<ASTN>& args) {
    // Get the AST of the function
        AslTree f = FuncName2Tree.get(funcname);
        if (f == null) throw new RuntimeException(" function " + funcname + " not declared");

        // Gather the list of arguments of the caller. This function
        // performs all the checks required for the compatibility of
        // parameters.
        ArrayList<Data> Arg_values = listArguments(f, args);

        // Dumps trace information (function call and arguments)
        if (trace != null) traceFunctionCall(f, Arg_values);
        
        // List of parameters of the callee
        AslTree p = f.getChild(1);
        int nparam = p.getChildCount(); // Number of parameters

        // Create the activation record in memory
        Stack.pushActivationRecord(funcname, lineNumber());

        // Track line number
        setLineNumber(f);
         
        // Copy the parameters to the current activation record
        for (int i = 0; i < nparam; ++i) {
            String param_name = p.getChild(i).getText();
            Stack.defineVariable(param_name, Arg_values.get(i));
        }

        // Execute the instructions
        Data result = executeListInstructions (f.getChild(2));

        // If the result is null, then the function returns void
        if (result == null) result = new Data();
        
        // Dumps trace information
        if (trace != null) traceReturn(f, result, Arg_values);
        
        // Destroy the activation record
        Stack.popActivationRecord();

        return result;
}

list<Data> Interpreter::listArguments(const freeling::tree<ASTN>& AstF, const freeling::tree<ASTN>& args) {
    if (args != null) setLineNumber(args);
        AslTree pars = AstF.getChild(1);   // Parameters of the function
        
        // Create the list of parameters
        ArrayList<Data> Params = new ArrayList<Data> ();
        int n = pars.getChildCount();

        // Check that the number of parameters is the same
        int nargs = (args == null) ? 0 : args.getChildCount();
        if (n != nargs) {
            throw new RuntimeException ("Incorrect number of parameters calling function " +
                                        AstF.getChild(0).getText());
        }

        // Checks the compatibility of the parameters passed by
        // reference and calculates the values and references of
        // the parameters.
        for (int i = 0; i < n; ++i) {
            AslTree p = pars.getChild(i); // Parameters of the callee
            AslTree a = args.getChild(i); // Arguments passed by the caller
            setLineNumber(a);
            if (p.getType() == AslLexer.PVALUE) {
                // Pass by value: evaluate the expression
                Params.add(i,evaluateExpression(a));
            } else {
                // Pass by reference: check that it is a variable
                if (a.getType() != AslLexer.ID) {
                    throw new RuntimeException("Wrong argument for pass by reference");
                }
                // Find the variable and pass the reference
                Data v = Stack.getVariable(a.getText());
                Params.add(i,v);
            }
        }
        return Params;
}

Data Interpreter::executeListInstructions(const freeling::tree<ASTN>& t) {
    Data result;
    for (int i = 0; i < t.num_children(); ++i) {
        result = executeInstruction (t.nth_child_ref(i));
        if (!result.isVoid()) return result;
    }
    return result;
}

Data Interpreter::executeInstruction(const freeling::tree<ASTN>& t) {
    Data value;
    freeling::tree<ASTN>::const_iterator it = t.begin();
    ASTN node = *it;
    wstring type = node.getToken();

    if (type == L"ASSIGMENT") {
        //evalua esquerra (id, array, o map)
        //evalua dreta
        //m_stack.top()[esquerra] = dreta;
        return value;
    }
    else if (type == L"DPLUS") {
        //suma 1 a value
        return value;
    }
    else if (type == L"DPLUSR") {
        return value;
        //suma 1 a value
    }
    else if (type == L"DMINUS") {
        //resta 1 a value
        return value;   
    }
    else if (type == L"DMINUSR") {
        return value;   
        //resta 1 a value
    }
    else if (type == L"IF") {
            value = evaluateExpression(t.getChild(0));
            checkBoolean(value);
            if (value.getBooleanValue()) return executeListInstructions(t.getChild(1));
            // Is there else statement ?
            if (t.getChildCount() == 3) return executeListInstructions(t.getChild(2));
            return null;
    }
    else if (type == L"IF ELSE") {

    }
    else if (type == L"IF ELSE IF") {

    }
    else if (type == L"IF ELSE IF ELSE") {

    }
    else if (type == L"ELSE IF") {

    }
    else if (type == L"ELSE") {

    }
    else if (type == L"WHILE") {
            while (true) {
                value = evaluateExpression(t.getChild(0));
                checkBoolean(value);
                if (!value.getBooleanValue()) return null;
                Data r = executeListInstructions(t.getChild(1));
                if (r != null) return r;
            }
    }
    else if (type == L"FOR") {

    }
    else if (type == L"DO WHILE") {

    }
    else if (type == L"RETURN") {
            if (t.getChildCount() != 0) {
                return evaluateExpression(t.getChild(0));
            }
            return new Data(); // No expression: returns void data
    }
    else if (type == L"PRINT") {
            AslTree v = t.getChild(0);
            // Special case for strings
            if (v.getType() == AslLexer.STRING) {
                System.out.format(v.getStringValue());
                return null;
            }

            // Write an expression
            System.out.print(evaluateExpression(v).toString());
            return null;
    }
    else if (type == L"FUNCALL") {
            executeFunction(t.getChild(0).getText(), t.getChild(1));
            return null;
    }
    else assert(false);
    return value;
}

Data Interpreter::evaluateExpression(const freeling::tree<ASTN>& t) {
    Data value, value2;
    freeling::tree<ASTN>::const_iterator it = t.begin();
    ASTN node = *it;
    wstring type = node.getToken();

    // Atoms
    if (type == L"TOKEN BOOL") {
                value = new Data(t.getBooleanValue());

    }
    else if (type == L"TOKEN INT") {
                value = new Data(t.getIntValue());

    }
    else if (type == L"TOKEN DOUBLE") {

    }
    else if (type == L"TOKEN STRING") {

    }
    else if (type == L"TOKEN ID") {
                value = new Data(Stack.getVariable(t.getText()));

    }
    else if (type == L"TOKEN OBJECT") {

    }
    else if (type == L"TOKEN ARRAY") {

    }
    else if (type == L"FUNCALL") {
                value = executeFunction(t.getChild(0).getText(), t.getChild(1));
                assert value != null;
                if (value.isVoid()) {
                    throw new RuntimeException ("function expected to return a value");
                }
    }
    // Unary operators
    else if (type == L"NOT") {
        assert(t.num_children() == 1);
                checkBoolean(value);
                value.setValue(!value.getBooleanValue());
    }
    else if (type == L"NEG") {
        assert(t.num_children() == 1);

        
                checkInteger(value);
                value.setValue(-value.getIntegerValue());
    } 
    else if (type == L"POS") {
        assert(t.num_children() == 1);

                checkInteger(value);
        
    } 
    else if (type == L"DPLUS") {
        assert(t.num_children() == 1);

            //suma 1 a value
            return value;
    } 
    else if (type == L"DPLUSR") {
        assert(t.num_children() == 1);

            return value;
            //suma 1 a value
    } 
    else if (type == L"DMINUS") {
        assert(t.num_children() == 1);

            //resta 1 a value
            return value;   
    }
    else if (type == L"DMINUSR") {
        assert(t.num_children() == 1);

            return value;   
            //resta 1 a value
    }
    // Relational operators
    else if (type == L"EQUAL" || type == L"NOT_EQUAL" || type == L"LT" || type == L"LE" || type == L"GT" || type == L"GE") {
        assert(t.num_children() == 2);

                value2 = evaluateExpression(t.getChild(1));
                if (value.getType() != value2.getType()) {
                  throw new RuntimeException ("Incompatible types in relational expression");
                }
                value = value.evaluateRelational(type, value2);
                break;
    }
    // Arithmetic operators
    else if (type == L"PLUS" || type == L"MINUS" || type == L"MULT" || type == L"DIV" || type == L"MOD") {
        assert(t.num_children() == 2);

                value2 = evaluateExpression(t.getChild(1));
                checkInteger(value); checkInteger(value2);
                value.evaluateArithmetic(type, value2);
                break;
    }
    // Boolean operators
    else if (type == L"AND" || type == L"OR") {
        assert(t.num_children() == 2);

            // is deferred (lazy, short-circuit evaluation).
            checkBoolean(value);
            value = evaluateBoolean(type, value, t.getChild(1));
            break;
    }
    // Array & Object operators
    else if (type == L"OBJET ACCES") {

    }
    else if (type == L"ARRAY ACCES") {

    }
    else if (type == L"ARRAY ADD") {

    }
    else if (type == L"ARRAY REMOVE") {

    }
    else if (type == L"ARRAY INDEXOF") {

    }
    else if (type == L"ARRAY SIZE") {

    }
    else assert(false);
    return value;
}

Data& Interpreter::evaluateBool(wstring type, Data& v, const freeling::tree<ASTN>& t) {
    if (type == L"AND") {
        if (!v.getBoolValue()) return v;
    }
    else if (type == L"OR") {
        if (v.getBoolValue()) return v;
    }
    else assert(false);

    v = evaluateExpression(t);
    checkBool(v);
    return v;
}

void Interpreter::checkBool(const Data& b) {
    if (!b.isBool()) {
        throw runtime_error("expecting Boolean expression");
    }
}

void Interpreter::checkNumeric(const Data& b) {
    if (!b.isInt() && !b.isDouble()) {
        throw runtime_error("expecting numerical expression");
    }
}
