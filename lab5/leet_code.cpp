
#include "suffix_tree_int.cpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>


class Solution {
public:
    int longestCommonSubpath(int n, vector<vector<int>>& paths) {
        int m = paths.size();
        vector<int> text;
        vector<int> owner;
        int sepBase = n;

        for (int i = 0; i < m; i++) {
            for (int city : paths[i]) {
                text.push_back(city);
                owner.push_back(i);
            }
            text.push_back(sepBase + i);
            owner.push_back(-1);
        }

        SuffixTree tree(text);

        int ans = 0;
        dfs(tree.root, text, owner, m, ans, 0);

        return ans;
    }

private:
    unordered_set<int> dfs(Node* node, const vector<int>& text, const vector<int>& owner, int m, int& ans, int depth) {
        unordered_set<int> seen;

        if (node->suffix_index != -1) {
            int idx = node->suffix_index;
            if (owner[idx] != -1)
                seen.insert(owner[idx]);
        }

        for (auto &kv : node->children) {
            Node* child = kv.second;
            int edgeLen = *(child->end) - child->start + 1;
            auto childSet = dfs(child, text, owner, m, ans, depth + edgeLen);
            if (seen.size() < childSet.size()) swap(seen, childSet);
            seen.insert(childSet.begin(), childSet.end());
        }

        if ((int)seen.size() == m) {
            ans = max(ans, depth);
        }

        return seen;
    }
};
