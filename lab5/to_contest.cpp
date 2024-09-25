#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

// уникальный символ, добавление которого гарантирует построение true suffix tree
// т.е. никакой суффикс не будет являться префиксом какого-либо другого суффикса
#define TERMINATION_SYMBOL '$'

using namespace std;

class SuffixTree;

class Node
{
public:
    friend SuffixTree;

    // конструктор ноды без индекса: используется при создании новых внутренних вершин
    Node(Node *link, int start, int *end) : Node(link, start, end, -1)
    {
    }

    // конструктор с индексом: исп-ся при создании листьев
    // ибо при их создании можно доподлинно определить их индекс
    Node(Node *link, int start, int *end, int ind) : suffix_link(link),
                                                     start(start),
                                                     end(end),
                                                     suffix_index(ind)
    {
    }

private:
    unordered_map<char, Node *> children; // "массив" детей
    Node *suffix_link;          // суффиксная ссылка
    int start;                  // номер первого символа вершины в тексте
    int *end;                   // номер последнего символа вершины в тексте
    int suffix_index;           // номер суффикса
};

class SuffixTree
{
public:
    SuffixTree(string &text);             // конструктор по строке
    void BuildSuffixTree();               // само построение дерева
    ~SuffixTree()                         // деструктор
    {
        DeleteSuffixTree(root);
    }
    string LexMinString(const size_t & n);
    string LexMinString(const int id, const size_t & n);

private:
    void ExtendSuffixTree(int pos);   // расширение дерева
    void DeleteSuffixTree(Node *node) // удаление дерева
    {
        for (auto it : node->children)
            DeleteSuffixTree(it.second);
        if (node->suffix_index == -1)
            delete node->end;
        delete node;
    }
    void CountIndex(Node *node, vector<int> &vec); // сбор индексов листьев в вектор, содержащихся в поддереве node
    int EdgeLength(Node *node);                    // подсчет длины подстроки расположенной на дуге

    Node *root = new Node(nullptr, -1, new int(-1)); // корень
    Node *lastCreatedInternalNode = nullptr;         // последняя созданная внутрення вершина

    string text; // текст, по которому строится суфтри

    Node *activeNode = nullptr;   // то, откуда начнется расширение на следующей фазе
    int activeEdge = -1;          // индекс символа, который задает движение из текущей ноды
    int activeLength = 0;         // на сколько символов ступаем в направлении activeEdge
    int remainingSuffixCount = 0; // сколько суффиксов осталось создать. по сути, сколько суффиксов на прошлом шаге мы не создали.
    int leafEnd = -1;             // глобальная переменная, определяет содержимое листьев: её инкремент как бы приписывает новый символ к ним
};


SuffixTree::SuffixTree(string &str) : text(str)
{
    text += TERMINATION_SYMBOL;
    BuildSuffixTree();
}

void SuffixTree::BuildSuffixTree()
{
    activeNode = root;
    for (size_t i = 0; i < text.length(); i++)
        ExtendSuffixTree(i);
}

int SuffixTree::EdgeLength(Node *node)
{
    return *(node->end) - (node->start) + 1;
}

void SuffixTree::ExtendSuffixTree(int phase)
{
    // установить lastCreatedInternalNode = null перед началом каждой фазы
    lastCreatedInternalNode = nullptr;

    // глобальный end для листьев
    // инкремент leafEnd воплощает правило 1, прием 3: наращивание листьев
    leafEnd++;

    // сколько суффиксов осталось создать
    remainingSuffixCount++;

    while (remainingSuffixCount > 0)
    {
        // если activeLength равна 0, тогда ищем текущий символ из корня
        if (activeLength == 0)
            activeEdge = phase; // индекс текущего символа в тексте определяет дугу, по которой будем двигаться

        // ищем текущий символ в начале исходящих из activeNode дуг
        auto find = activeNode->children.find(text[activeEdge]);

        // не нашли
        if (find == activeNode->children.end())
        {
            // добавим новую листовую дугу, исходящую из activeNode, начинающуся текущим символом
            activeNode->children.insert(make_pair(
                text[activeEdge],
                new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1)));
            // и коль скоро создали новую внутр. вершинку, установим на нее суффлинку последней созданной внутренней вершины
            if (lastCreatedInternalNode != nullptr)
            {
                lastCreatedInternalNode->suffix_link = activeNode;
                lastCreatedInternalNode = nullptr;
            }
        }
        else
        {
            // а если-таки есть дуга из activeNode, начинающаяся текущим символом, пойдем по ней спускаться
            Node *next = find->second;
            int edge_length = EdgeLength(next);

            // спуск по дуге(-ам)
            if (activeLength >= edge_length)
            {
                activeEdge += edge_length;
                activeLength -= edge_length;
                activeNode = next;
                continue; // таким образом мы будем спускаться, покуда не станет activeLength < edge_length
            }

            // правило 3: если текущий символ есть на дуге,
            // т.е. суффикс уже есть в дереве, то просто увеличим activeLength
            // как бы "шагнем вперед" по дуге
            if (text[next->start + activeLength] == text[phase])
            {
                // если lastCreatedInternalNode != null
                // это означает, что 2-е правило было применено ранее (создание новой вн. вершины)
                // установим суффлинку в activeNode
                if (lastCreatedInternalNode != nullptr && activeNode != root)
                    lastCreatedInternalNode->suffix_link = activeNode;
                activeLength++;
                break; // выйдем из цикла while
            }

            // сюда попали, если текущего символа нет на дуге
            // создадим новую внутреннюю вершинку
            Node *split = new Node(root, next->start, new int(next->start + activeLength - 1));
            // подвесим к activeNode новую вершинку
            activeNode->children[text[activeEdge]] = split;
            // у "следующей" вершинки изменим, очевидно, начало, ибо мы ее "обрубили"
            next->start += activeLength;
            // подвесим новую листовую вершинку
            split->children.insert(make_pair(text[phase], new Node(root, phase, &leafEnd, phase - remainingSuffixCount + 1)));
            // подвесим отрубленную вершинку
            split->children.insert(make_pair(text[next->start], next));
            // и не забудем про установку ссылок при создании новой внутренней вершины
            if (lastCreatedInternalNode != nullptr)
                lastCreatedInternalNode->suffix_link = split;
            lastCreatedInternalNode = split;
        }

        remainingSuffixCount--;

        // если activeNode == root, тогда согласно правилу 2, мы декр. activeLength и инкр. activeEdge
        // это, можно сказать, альтернатива суфф. ссылке в случае, когда activeNode == root
        if (activeNode == root && activeLength > 0)
        {
            activeLength--;
            activeEdge++;
        }
        else if (activeNode != root) // если же activeNode != root, то радостно скачем по имеющейся суффиксной ссылке
            activeNode = activeNode->suffix_link;
    }
}

// запушить в вектор листья (вернее, их индексы), которые находятся в поддереве с корнем node
void SuffixTree::CountIndex(Node *node, vector<int> &vec)
{
    if (!node) // конец рекурсии: пустое поддерево
        return;
    for (auto it : node->children) // рекурсивный вызов для всех поддеревьев
        CountIndex(it.second, vec);
    if (node->suffix_index != -1) // как дошли до листа, добавляем в вектор его индекс
        vec.push_back(node->suffix_index);
}

string SuffixTree::LexMinString(const size_t &n)
{
    vector<int> suffixIndices;
    CountIndex(root, suffixIndices); // Собираем все индексы суффиксов

    int minIndex = -1; // Индекс минимального суффикса
    for (const auto &index : suffixIndices)
    {
        // Проверяем, что индекс суффикса находится в первой половине удвоенной строки
        if (index < n)
        {
            // Если минимальный суффикс еще не найден или текущий суффикс меньше минимального
            if (minIndex == -1 || text.compare(index, n, text, minIndex, n) < 0)
            {
                minIndex = index;
            }
        }
    }

    // Возвращаем минимальный циклический сдвиг длиной n
    return (minIndex != -1) ? text.substr(minIndex, n) : "";
}


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    string s;
    cin >> s;
    int n = s.length();
    s += s;

    SuffixTree tree(s);
    cout << tree.LexMinString(n) << endl;

    return 0;
}
