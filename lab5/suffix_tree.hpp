#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

struct SuffixTreeNode {
    unordered_map<char, SuffixTreeNode*> Children;
    int Start, *End;
    SuffixTreeNode* SuffixLink;
    int SuffixIndex;
    bool IsDynamicEnd;

    SuffixTreeNode(int start, int* end, bool isDynamicEnd = false)
        : Start(start), End(end), SuffixLink(nullptr), SuffixIndex(-1), IsDynamicEnd(isDynamicEnd) {}

    ~SuffixTreeNode() {
        if (End != nullptr && IsDynamicEnd) {
            delete End;
        }
        for (auto& child : Children) {
            delete child.second;
        }
    }
};

class SuffixTree {
private:
    string S;
    SuffixTreeNode* Root;
    SuffixTreeNode* LastNewNode;
    SuffixTreeNode* ActiveNode;
    int ActiveEdge;
    int ActiveLength;
    int RemainingSuffixCount;
    int LeafEnd;
    int* RootEnd;
    int* SplitEnd;
    int Size;

    SuffixTreeNode* NewNode(int start, int* end, bool isDynamic = false) {
        SuffixTreeNode* node = new SuffixTreeNode(start, end, isDynamic);
        node->SuffixLink = Root;
        return node;
    }

    int EdgeLength(SuffixTreeNode* n) {
        return *(n->End) - (n->Start) + 1;
    }

    bool WalkDown(SuffixTreeNode* currNode) {
        if (ActiveLength >= EdgeLength(currNode)) {
            ActiveEdge += EdgeLength(currNode);
            ActiveLength -= EdgeLength(currNode);
            ActiveNode = currNode;
            return true;
        }
        return false;
    }

    void ExtendSuffixTree(int pos) {
        LeafEnd = pos;
        RemainingSuffixCount++;
        LastNewNode = nullptr;

        while (RemainingSuffixCount > 0) {
            if (ActiveLength == 0) {
                ActiveEdge = pos;
            }

            if (ActiveNode->Children.find(S[ActiveEdge]) == ActiveNode->Children.end()) {
                ActiveNode->Children[S[ActiveEdge]] = NewNode(pos, &LeafEnd, false);

                if (LastNewNode != nullptr) {
                    LastNewNode->SuffixLink = ActiveNode;
                    LastNewNode = nullptr;
                }
            } else {
                SuffixTreeNode* next = ActiveNode->Children[S[ActiveEdge]];
                if (WalkDown(next)) {
                    continue;
                }

                if (S[next->Start + ActiveLength] == S[pos]) {
                    if (LastNewNode != nullptr && ActiveNode != Root) {
                        LastNewNode->SuffixLink = ActiveNode;
                        LastNewNode = nullptr;
                    }
                    ActiveLength++;
                    break;
                }

                SplitEnd = new int;
                *SplitEnd = next->Start + ActiveLength - 1;

                SuffixTreeNode* split = NewNode(next->Start, SplitEnd, true);
                ActiveNode->Children[S[ActiveEdge]] = split;

                split->Children[S[pos]] = NewNode(pos, &LeafEnd, false);
                next->Start += ActiveLength;
                split->Children[S[next->Start]] = next;

                if (LastNewNode != nullptr) {
                    LastNewNode->SuffixLink = split;
                }

                LastNewNode = split;
            }

            RemainingSuffixCount--;

            if (ActiveNode == Root && ActiveLength > 0) {
                ActiveLength--;
                ActiveEdge = pos - RemainingSuffixCount + 1;
            } else if (ActiveNode != Root) {
                ActiveNode = ActiveNode->SuffixLink;
            }
        }
    }

    void SetSuffixIndexByDFS(SuffixTreeNode* n, int labelHeight) {
        if (n->SuffixIndex == -1) {
            for (auto& child : n->Children) {
                SetSuffixIndexByDFS(child.second, labelHeight + EdgeLength(child.second));
            }
        } else if (n->Children.empty()) {
            n->SuffixIndex = Size - labelHeight;
        }
    }

public:
    SuffixTree(const string& s) : S(s), Size(s.size()) {
        RootEnd = new int(-1);
        Root = NewNode(-1, RootEnd);
        ActiveNode = Root;
        ActiveEdge = -1;
        ActiveLength = 0;
        RemainingSuffixCount = 0;
        LeafEnd = -1;

        for (int i = 0; i < Size; i++) {
            ExtendSuffixTree(i);
        }

        int labelHeight = 0;
        SetSuffixIndexByDFS(Root, labelHeight);
    }

    // Функция для поиска минимальной циклической перестановки
    string GetMinimalCyclicShift() {
        int n = S.size() / 2;
        int best_start = 0;

        // Минимизируем индекс начала сдвига, работая с индексами, а не строками
        for (int i = 1; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (S[best_start + j] != S[i + j]) {
                    if (S[i + j] < S[best_start + j]) {
                        best_start = i;
                    }
                    break;
                }
            }
        }

        return S.substr(best_start, n);
    }

    ~SuffixTree() {
        delete RootEnd;
        delete Root;
    }
};