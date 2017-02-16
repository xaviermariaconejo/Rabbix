#include <iostream>
#include <vector>
#include <list>
#include "ASTN.h"
#include "tree.h"

using namespace std;
using namespace ATN;
using namespace freeling;

int main() {
	/*
	tree<int> t0(0);
	tree<int> t1(1);
	tree<int> t2(2);
	tree<int> t3(3);
	tree<int> t4(4);
	tree<int> t5(5);

	tree<int>::const_iterator it = t1.begin();

	cout << "TEST TREE" << endl << "T1 info: " << *it << endl << "T1 num childs: " << t1.num_children() <<endl;

	t0.add_child(t1);

	it = t0.begin();

	cout << "T0 num childs: " << t0.num_children() << endl;

	cout << *it << endl;

	it = it.nth_child(0);

	cout << *it << endl;

	it = it.get_parent();

	cout << *it << endl;

	t0.add_child(t2);
	t0.add_child(t3);
	t0.add_child(t4);
	t0.add_child(t5);


	it = it.nth_child(3);

	cout << *it << endl;
	++it;
	cout << *it << endl;
	++it;
	cout << *it << endl;

	tree<ASTN> t(ASTN(L"TOKEN"));
	*/

	/*
	std::list<int> l;
	l.push_back(1);
	*/

	string s = "INFO";
	wstring ws(s.begin(), s.end());
	cout << "TESTING: " << s << " " << string(ws.begin(), ws.end()) << endl;
 	tree<ASTN> t(ASTN(L"TOKEN", wstring(s.begin(), s.end())));
	tree<ASTN>::const_iterator it = t.begin();
	ASTN node = *it;
	wstring token = node.getToken();
	cout << string(token.begin(), token.end()) << endl;
}