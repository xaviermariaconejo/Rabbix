#include <iostream>
#include "atn.h"

using namespace ATN;
using namespace std;

int main(int argc, char **argv) {
    wstring_convert< codecvt_utf8_utf16<wchar_t> > converter;               // Converer wstring - string

    // Atn i;
    Atn i(L"numbers_es.atn");
    // Atn<vector<string>, string> i;
	// i.file(L"test4.atn");

// i.parse();

	// V es un container (vector, list,...) que conte elements de tipus T
	// V ha de ser indexable per posicio mb un operador []
 	// Atn<V, T>

    cout << i.str() << endl;

    string num;
    vector<wstring> v;
    while(cin >> num && num != "-1") {
        v.push_back(converter.from_bytes(num));
    }

    vector<Atn::Output> output = i.run(v);

    cout << endl << "Output principal main" << endl;
    for (int k = 0; k < output.size(); ++k) {
    	Atn::Output out = output[k];
    	cout << "    INIT: " << out.init << endl;
    	cout << "    FINAL: " << out.final << endl;
    	wstring ws = out.info;
    	cout << "    INFO: " << converter.to_bytes(ws) << endl << endl;
    } 
}
