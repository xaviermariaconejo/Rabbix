#include <iostream>
#include "atn.h"

using namespace ATN;
using namespace std;

int main(int argc, char **argv) {
    // Atn i;
    Atn i(L"numbers_es.atn");
    // Atn<vector<string>, string> i;
	// i.file(L"test4.atn");

// i.parse();

	// V es un container (vector, list,...) que conte elements de tipus T
	// V ha de ser indexable per posicio mb un operador []
 	// Atn<V, T>

    cout << i.str() << endl;

    vector<wstring> v = {};
    vector<Atn::Output> output = i.run(v);

    cout << endl << "Output principal main" << endl;
    for (int k = 0; k < output.size(); ++k) {
    	Atn::Output out = output[k];
    	cout << "    INIT: " << out.init << endl;
    	cout << "    FINAL: " << out.final << endl;
    	wstring ws = out.info;
    	cout << "    INFO: " << string(ws.begin(), ws.end()) << endl;
    } 
}
