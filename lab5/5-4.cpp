#include "suffix_tree.hpp"
#include <iostream>

using namespace std;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    string s;
    cin >> s;
    s += s;

    SuffixTree tree(s);
    cout << tree.GetMinimalCyclicShift() << endl;

    return 0;
}
