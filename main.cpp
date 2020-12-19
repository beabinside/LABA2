#include "mainwindow.h"
#include <optional>
#include <algorithm>
#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include "prefix_tree.h"

#include <QApplication>

int main() {
    prefix_tree<int> tree1;
    assert(tree1.size() == 0);
    assert(!tree1.find("a"));
    tree1.add("a", 1);
    assert(tree1.find("a"));
    assert(tree1.find_keys("") == vector<string>{ "a" });
    tree1.remove("a");
    assert(tree1.size() == 0);
    assert(!tree1.find("a"));
    assert(tree1.find_keys("") == vector<string>{ });

    tree1.add("aa", 1);
    tree1.add("ab", 1);
    tree1.add("ac", 1);
    assert(tree1.size() == 3);
    assert((tree1.find_keys("") == vector<string>{ "aa", "ab", "ac" }));
    assert((tree1.find_keys("a") == vector<string>{ "aa", "ab", "ac" }));

    tree1.add("aaa", 1);
    tree1.add("b", 1);
    tree1.add("cab", 1);
    assert(tree1.size() == 6);
#define help_output(x) cout << (#x) << " = " << (x) << endl
    //help_output(tree1.find_keys(""));
    assert((tree1.find_keys("") == vector<string>{ "aa", "aaa", "ab", "ac", "b", "cab" }));
    assert((tree1.find_keys("a") == vector<string>{ "aa", "aaa", "ab", "ac" }));
    assert((tree1.find_keys("aa") == vector<string>{ "aa", "aaa"}));
    assert((tree1.find_keys("ab") == vector<string>{ "ab"}));
    assert((tree1.find_keys("ac") == vector<string>{ "ac"}));
    assert((tree1.find_keys("b") == vector<string>{ "b"}));
    assert((tree1.find_keys("c") == vector<string>{ "cab"}));
    assert((tree1.find_keys("ca") == vector<string>{ "cab"}));
    assert((tree1.find_keys("cab") == vector<string>{ "cab"}));


    tree1.add("a", 1);
    assert(tree1["a"] == 1);
    assert(tree1["aa"] == 1);
    tree1["a"] = 2;
    assert(tree1["a"] == 2);
    assert(tree1["aa"] == 1);
    tree1.remove("a");
    tree1.add("a", 5);
    assert(tree1["a"] == 5);
    tree1.remove("a");

    assert(!tree1.find("a"));
    assert(tree1.find("aa"));
    assert(tree1.find("aaa"));
    assert(tree1.find("ab"));
    assert(!tree1.find("abb"));
    assert(!tree1.find("abc"));
    assert(tree1.find("ac"));
    assert(!tree1.find("aca"));
    assert(!tree1.find("acb"));
    assert(!tree1.find("acc"));

    assert(!tree1.find("bca"));
    assert(!tree1.find("bcb"));
    assert(!tree1.find("bcc"));
    assert(tree1.find("b"));
    assert(!tree1.find("c"));
    assert(tree1.find("cab"));
    assert(!tree1.find("d"));

    tree1.add("a", 1);
    assert(tree1.find("a"));
    tree1.remove("a");

    assert(!tree1.find("a"));
    assert(tree1.find("aa"));
    assert(tree1.find("aaa"));
    assert(tree1.find("ab"));
    assert(!tree1.find("abb"));
    assert(!tree1.find("abc"));
    assert(tree1.find("ac"));
    assert(!tree1.find("aca"));
    assert(!tree1.find("acb"));
    assert(!tree1.find("acc"));

    assert(!tree1.find("bca"));
    assert(!tree1.find("bcb"));
    assert(!tree1.find("bcc"));
    assert(tree1.find("b"));
    assert(!tree1.find("c"));
    assert(tree1.find("cab"));
    assert(!tree1.find("d"));


    assert(prefix_tree<int>() == prefix_tree<int>());
    assert(tree1 != prefix_tree<int>());
    assert(prefix_tree<int>() != tree1);
    assert(tree1 == tree1);

    prefix_tree<int> A, B;
    A.add("a", 1);
    B.add("a", 2);
    assert(A != B);
    B.add("b", 2);
    assert(A != B);
    B.remove("b");
    assert(A != B);
    B["a"] = 1;
    assert(A == B);


    prefix_tree<int> C;
    ostringstream sout1;
    C.out(sout1);
    assert(sout1.str() == "0\n");

    istringstream sin("3\na\n3b\n1\naa\n5\n");
    C.in(sin);
    assert(C.size() == 3);
    assert((C.find_keys("") == vector<string>{ "a", "aa", "b" }));
    assert(C["a"] == 3);
    assert(C["aa"] == 5);
    assert(C["b"] == 1);

    ostringstream sout2;
    C.out(sout2);
    assert(sout2.str() == "3\na\n3\naa\n5\nb\n1\n");

    return 0;
}
