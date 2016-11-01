#include <iostream>
#include "tree.h"
#include "node.h"

using namespace std;

int main() {
	cout << "TEST STARTS" << endl;
	node n;
	n.setToken(L"INT");
	n.setValueInt(1);
	wcout << n.getToken() << ": " << n.getValueInt() << endl;

	freeling::tree<node> t;
}