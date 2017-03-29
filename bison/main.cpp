#include <iostream>
//#include "scanner.h"
//#include "parser.hpp"
#include "atn.h"

using namespace ATN;
using namespace std;

int main(int argc, char **argv) {
    Atn i;
    int res = i.parse();
    cout << "Parse complete. Result = " << res << endl << endl;
    cout << i.str() << endl;

    vector<wstring> v = {L"un", L"dos", L"tres"};
    i.run(v);
    return res;
}
