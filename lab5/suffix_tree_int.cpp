#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

#define TERMINATION_SYMBOL -1

using namespace std;

class SuffixTree;

class Node {
public:
    friend SuffixTree;

    Node(Node *link, int start, int *end) 
        : Node(link, start, end, -1) {}

    Node(Node *link, int start, int *end, int ind) 
        : suffix_link(link),
          start(start),
          end(end),
          suffix_index(ind) {}

    unordered_map<int, Node*> children;
    Node *suffix_link;
    int start;
    int *end;
    int suffix_index;
};

class SuffixTree {
public:
    SuffixTree(vector<int> &text) : text(text) {
        text.push_back(TERMINATION_SYMBOL);
        BuildSuffixTree();
    }

    void BuildSuffixTree();
    ~SuffixTree() {
        DeleteSuffixTree(root);
    }

    vector<int> LexMinString(const size_t &n);
    vector<int> LexMinString(Node *node, const size_t &n);

    void ExtendSuffixTree(int pos);
    void DeleteSuffixTree(Node *node) {
        for (auto it : node->children)
            DeleteSuffixTree(it.second);
        if (node->suffix_index == -1)
            delete node->end;
        delete node;
    }
    void CountIndex(Node *node, vector<int> &vec);
    int EdgeLength(Node *node);

    Node *root = new Node(nullptr, -1, new int(-1));
    Node *lastCreatedInternalNode = nullptr;

    vector<int> text;
    Node *activeNode = nullptr;
    int activeEdge = -1;
    int activeLength = 0;
    int remainingSuffixCount = 0;
    int leafEnd = -1;
};

void SuffixTree::BuildSuffixTree() {
    activeNode = root;
    for (size_t i = 0; i < text.size(); i++)
        ExtendSuffixTree(i);
}

int SuffixTree::EdgeLength(Node *node) {
    return *(node->end) - (node->start) + 1;
}

void SuffixTree::ExtendSuffixTree(int phase) {
    lastCreatedInternalNode = nullptr;
    leafEnd++;
    remainingSuffixCount++;

    while (remainingSuffixCount > 0) {
        if (activeLength == 0)
            activeEdge = phase;
        auto find = activeNode->children.find(text[activeEdge]);
        if (find == activeNode->children.end()) {
            activeNode->children.insert(make_pair(
                text[activeEdge],
                new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1)));
            if (lastCreatedInternalNode != nullptr) {
                lastCreatedInternalNode->suffix_link = activeNode;
                lastCreatedInternalNode = nullptr;
            }
        } else {
            Node *next = find->second;
            int edge_length = EdgeLength(next);
            if (activeLength >= edge_length) {
                activeEdge += edge_length;
                activeLength -= edge_length;
                activeNode = next;
                continue;
            }
            if (text[next->start + activeLength] == text[phase]) {
                if (lastCreatedInternalNode != nullptr && activeNode != root)
                    lastCreatedInternalNode->suffix_link = activeNode;
                activeLength++;
                break;
            }
            Node *split = new Node(root, next->start, new int(next->start + activeLength - 1));
            activeNode->children[text[activeEdge]] = split;
            next->start += activeLength;
            split->children.insert(make_pair(text[phase], new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1)));
            split->children.insert(make_pair(text[next->start], next));
            if (lastCreatedInternalNode != nullptr)
                lastCreatedInternalNode->suffix_link = split;
            lastCreatedInternalNode = split;
        }

        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) {
            activeLength--;
            activeEdge++;
        } else if (activeNode != root)
            activeNode = activeNode->suffix_link;
    }
}

void SuffixTree::CountIndex(Node *node, vector<int> &vec) {
    if (!node) return;
    for (auto it : node->children)
        CountIndex(it.second, vec);
    if (node->suffix_index != -1)
        vec.push_back(node->suffix_index);
}

vector<int> SuffixTree::LexMinString(const size_t &n) {
    if (n == 0) return {};

    Node* currentNode = root;
    size_t remainingN = n;
    vector<int> result;

    while (remainingN > 0 && currentNode != nullptr) {
        int lexMinChar = INT32_MAX;
        Node* lexMinNode = nullptr;

        for (auto &child : currentNode->children) {
            if (child.first != TERMINATION_SYMBOL && child.first < lexMinChar) {
                lexMinChar = child.first;
                lexMinNode = child.second;
            }
        }

        if (lexMinNode == nullptr) {
            if (currentNode->suffix_link != nullptr) {
                currentNode = currentNode->suffix_link;
                continue;
            } else {
                break;
            }
        }

        int edgeLen = EdgeLength(lexMinNode);

        if (remainingN <= static_cast<size_t>(edgeLen)) {
            for (int i = 0; i < (int)remainingN; i++)
                result.push_back(text[lexMinNode->start + i]);
            break;
        } else {
            for (int i = 0; i < edgeLen; i++)
                result.push_back(text[lexMinNode->start + i]);
            remainingN -= edgeLen;
            currentNode = lexMinNode;
        }
    }

    return result;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    string s;
    cin >> s;
    int n = s.length();

    vector<int> seq;
    for (char c : s) seq.push_back((unsigned char)c);

    seq.insert(seq.end(), seq.begin(), seq.end()); // удвоение

    SuffixTree tree(seq);

    auto res = tree.LexMinString(n);

    for (int v : res) cout << char(v);
    cout << "\n";

    return 0;
}
