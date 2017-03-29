#include <iostream>
#include "atn.h"

using namespace ATN;
using namespace std;

int main(int argc, char **argv) {
    Atn i;
	// i.file(L"test4.atn");

    int res = i.parse();
    cout << "Parse complete. Result = " << res << endl << endl;
    cout << i.str() << endl;

    vector<wstring> v = {L"un", L"dos", L"tres"};
    vector<Atn::Output> output = i.run(v);

    cout << endl << "Output principal main" << endl;
    for (int k = 0; k < output.size(); ++k) {
    	Atn::Output out = output[k];
    	cout << "    INIT: " << out.init << endl;
    	cout << "    FINAL: " << out.final << endl;
    	wstring ws = out.info;
    	cout << "    INFO: " << string(ws.begin(), ws.end()) << endl;
    } 
    return res;
}
