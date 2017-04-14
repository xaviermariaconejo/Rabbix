#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <locale>
#include <codecvt>
#include <fstream>
#include <sstream>
#include "data.h"
#include "astn.h"
#include "atnn.h"
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

	/*
	string s = "INFO";
	wstring ws(s.begin(), s.end());
	cout << "TESTING: " << s << " " << string(ws.begin(), ws.end()) << endl;
 	tree<ASTN> t(ASTN(L"TOKEN", wstring(s.begin(), s.end())));
	tree<ASTN>::const_iterator it = t.begin();
	ASTN node = *it;
	wstring token = node.getToken();
	cout << string(token.begin(), token.end()) << endl;
	*/

	/*
	wstring a = L"A";
	wstring b = L"z";
	if (a <  b) cout << "YES" << endl;
	else cout << "NO" << endl;

	map<wstring, int> m;
	m[L"HI"] = 2;
	cout << "YEY " << m[L"HI"] << endl; 
	auto it = m.find(L"asdf");
	if (it == m.end()) cout << "yea" << endl;
	it = m.find(L"HI");
	cout << it->second << endl;
	*/


	/*
    wstring_convert< codecvt_utf8_utf16<wchar_t> > converter;
	
	string s = "és ñ?";
	wstring ws = converter.from_bytes(s);
	s = converter.to_bytes(ws);
	cout << "TEST WS " << s << endl;
	
	wstring s1 = L"3";
	wstring s2 = L"20";
	if (s1 < s2) cout << "COOL" << endl;
	else cout << "JAJA" << endl;
	*/

	
	/*
	string s = "safas \n";
	cout << s.length() << " " << s.substr(1, s.length() - 2) << endl << endl;
	printf("anb %s afsfafa \n", s.c_str());
	*/

	/*
	string s = "abcdef yeah";
	cout << s.find("h") << endl << s.find("z") << endl << s.substr(0, s.find("z")) << endl;
	cout << endl << s[0] << endl;
	wstring ws = L"abcd";
	ws = ws[1];
	cout << string(ws.begin(), ws.end()) << endl;
	*/

	
	// map<int, Data* > m;
	// stack< map<int, Data*> > st;
	// st.push(m);

	// Data* d = new Data(3);
	// cout << d->isInt() << " " << d->isDouble() << endl;
	// st.top()[0] = d;
	// cout << (st.top()[0])->isInt() << " " << (st.top()[0])->isDouble() << endl;
	// Data* d2 = st.top()[0];
	// d2->setDoubleValue(1.0);
	// cout << d->isInt() << " " << d->isDouble() << endl;

	// cout << (st.top()[0])->isInt() << " " << (st.top()[0])->isDouble() << endl;
	

	// wstring ws = L"JO:LA";
	// cout << string(ws.begin(), ws.end()) << endl;
	// wstring sub = ws.substr(0,1);
	// cout << string(sub.begin(), sub.end()) << endl;
	// sub = ws.substr(0);
	// cout << string(sub.begin(), sub.end()) << endl;

	/*
	//TEST DATA
	Data* d1 = new Data(1.5);
	Data* d2 = new Data(2.0);
	Data* d3 = new Data(1);
	cout << d1->isDouble() << " " << d2->isDouble() << " " << d3->isDouble() << " " << d3->isInt() << endl;
	cout << d1->getDoubleValue() << " " << d2->getDoubleValue() << " " << d3->getIntValue() << endl;
	d1->evaluateArithmetic(L"PLUS",d2);
	cout << d1->isDouble() << " " << d2->isDouble() << " " << d3->isDouble() << " " << d3->isInt() << endl;
	cout << d1->getDoubleValue() << " " << d2->getDoubleValue() << " " << d3->getIntValue() << endl;
	d2->evaluateArithmetic(L"MULT", d1);
	cout << d1->isDouble() << " " << d2->isDouble() << " " << d3->isDouble() << " " << d3->isInt() << endl;
	cout << d1->getDoubleValue() << " " << d2->getDoubleValue() << " " << d3->getIntValue() << endl;
	d1->evaluateArithmetic(L"MINUS", d2);
	cout << d1->isDouble() << " " << d2->isDouble() << " " << d3->isDouble() << " " << d3->isInt() << endl;
	cout << d1->getDoubleValue() << " " << d2->getDoubleValue() << " " << d3->getIntValue() << endl;
	d3->evaluateArithmetic(L"DIV", d2);
	cout << d1->isDouble() << " " << d2->isDouble() << " " << d3->isDouble() << " " << d3->isInt() << endl;
	cout << d1->getDoubleValue() << " " << d2->getDoubleValue() << " " << d3->getDoubleValue() << endl;
	*/

	// wstring s = L"hi there \n asdasfds";
	// cout << string(s.begin(), s.end()) << endl;


	// stack< map<int, Data*> > m_stack;
	// map<int, Data*> m;
	// m[0] = new Data(0);
	// m[1] = new Data(1);
	// m[2] = new Data(2);
	// m_stack.push(m);

	// map<int, Data*> Params;
	// Data* v0 = m_stack.top()[0]
	// Data* v1 = m_stack.top()[1]
	// Data* v2 = m_stack.top()[2]
	// Params[0] = v0;
	// Params[1] = v1;
	// Params[2] = v2;

	// m_stack.push(Params);
	// m_stack.top()[0];

	// union node {
	// 	int n;
	// 	double x;
	// };

	// node a = {0};
	// node b = {0, 1.0};

	// tree<node> t1(a);
	// tree<node> t2(b);
	// t1.add_child(t2);

	// ATNN::Node n = {new ASTN(L"ID", L"HI")};
	// tree<ATNN::Node>* obj = new tree<ATNN::Node>(n);

	// ASTN node = *(n.astn);
	// wstring ws = node.getToken();
	// cout << string(ws.begin(), ws.end()) << endl;

	// auto it = obj->begin();
	// ATNN::Node n1 = *it;
	// ASTN* n2 = (obj->begin())->astn;

	// ATNN::Node n = {new ASTN(L"ID")};
	// n.atn = nullptr;
	// n.astn = new ASTN(L"ID");
	// if (n.astn == nullptr) cout << "1" << endl;
	// else { cout <<  "1.1" << endl; ASTN n1 = *(n.astn); }
	// if (n.atn == nullptr) cout << "2" << endl;
	// else { cout << "2.2" << endl; ATNN* n2 = n.atn; if (n2 == NULL) cout << "HI THERE"; }

	// ifstream file( "test4.atn" );
 //    if (file)
 //    {
 //        stringstream buffer;
 //        buffer << file.rdbuf();
 //        file.close();

 //        cout << buffer.str() << endl << endl;
 //    }
}