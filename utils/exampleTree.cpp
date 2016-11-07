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

	freeling::tree<int> t(1);
	t.add_child(freeling::tree<int> (2));
	t.add_child(freeling::tree<int> (3));
	t.add_child(freeling::tree<int> (4));

	cout << "IS ROOT: " << t.is_root() << " NUM CHILD: " << t.num_children() << " HAS ANCESTOR 5: " << t.has_ancestor(freeling::tree<int>(5)) << " HAS ANCESTOR 4: " << t.has_ancestor(freeling::tree<int> (2)) << endl;

	freeling::tree_preorder_iterator<int> it;
	freeling::const_tree_preorder_iterator<int> it_const;
	it = t.begin();
	cout << "IT: " << *it << endl;
	it = it.sibling_rbegin();
	cout << "IT: " << *it << endl;
	--it;
	cout << "IT: " << *it << endl;
	it = it.get_parent();
	cout << "IT: " << *it << endl;
}