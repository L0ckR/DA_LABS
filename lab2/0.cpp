#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <vector>


void toLowerCase(char* str) {
    for (int i = 0; i <= 256 && str[i]; i++) {
        str[i] = std::tolower(static_cast<unsigned char>(str[i]));
    }
}

struct Node {
    char* key;
    uint64_t value;  
    uint64_t priority;
    Node* left;
    Node* right;

    Node(const char* k, uint64_t v, uint64_t p) : value(v), priority(p), left(nullptr), right(nullptr) {
        key = new char[strlen(k) + 1];
        strcpy(key, k);
    }
    
    ~Node() {
        delete[] key;
    }
};

class Treap {
    Node* root;

public:
    Treap() : root(nullptr) {}

    ~Treap() {
        clear(root);
    }

    void clear(Node* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }
    void saveToFile(Node* node, std::ofstream &file) {
        if (!node) return;

        size_t keyLen = strlen(node->key);
        file.write(reinterpret_cast<char*>(&keyLen), sizeof(keyLen)); 
        file.write(node->key, keyLen); 
        file.write(reinterpret_cast<char*>(&node->value), sizeof(node->value)); 
        file.write(reinterpret_cast<char*>(&node->priority), sizeof(node->priority)); 

        bool hasLeft = node->left != nullptr;
        bool hasRight = node->right != nullptr;
        file.write(reinterpret_cast<char*>(&hasLeft), sizeof(hasLeft)); 
        file.write(reinterpret_cast<char*>(&hasRight), sizeof(hasRight)); 

        if (node->left) saveToFile(node->left, file);
        if (node->right) saveToFile(node->right, file);
    }


    const char* save(const char* filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) return "ERROR: Cannot open file for writing";

        saveToFile(root, file);
        if (!file) return "ERROR: Writing to file failed";

        file.close();
        return "OK";
    }

    void restoreFromNodes(std::vector<Node*>& nodes) {
        if (nodes.empty()) return;
        
        root = nodes[0];
        std::vector<Node*> stack = {root};
        
        for (size_t i = 1; i < nodes.size(); ++i) {
            Node* current = nodes[i];
            Node* parent = nullptr;

            while (!stack.empty() && stack.back()->priority < current->priority) {
                parent = stack.back();
                stack.pop_back();
            }

            if (parent) {
                parent->right = current;
            }

            if (!stack.empty()) {
                stack.back()->left = current;
            }

            stack.push_back(current);
        }
    }

    Node* loadFromFile(std::ifstream &file) {
        size_t keyLen;
        if (!file.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen))) {  
            std::cerr << "Failed to read key length. File might be corrupted or empty." << std::endl;
            return nullptr;
        }

        char* key = new char[keyLen + 1];
        if (!file.read(key, keyLen)) { 
            std::cerr << "Failed to read key. Expected length: " << keyLen << std::endl;
            delete[] key;
            return nullptr;
        }
        key[keyLen] = '\0';

        uint64_t value;
        uint64_t priority;
        if (!file.read(reinterpret_cast<char*>(&value), sizeof(value)) ||  
            !file.read(reinterpret_cast<char*>(&priority), sizeof(priority))) {  
            std::cerr << "Failed to read value or priority. Key: " << key << std::endl;
            delete[] key;
            return nullptr;
        }

        Node* node = new Node(key, value, priority);
        delete[] key;

        bool hasLeft, hasRight;
        if (!file.read(reinterpret_cast<char*>(&hasLeft), sizeof(hasLeft)) ||  
            !file.read(reinterpret_cast<char*>(&hasRight), sizeof(hasRight))) {  
            std::cerr << "Failed to read left/right flags. Key: " << key << std::endl;
            delete node;
            return nullptr;
        }

        if (hasLeft) node->left = loadFromFile(file); 
        if (hasRight) node->right = loadFromFile(file);  

        return node;
    }

    const char* load(const char* filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return "ERROR: Cannot open file for reading";

        Node* newRoot = loadFromFile(file);
        if (!file || !newRoot) {
            file.close();
            return "ERROR: Invalid file format or reading failed";
        }

        clear(root);
        root = newRoot;

        file.close();
        return "OK";
    }


    Node* find(Node* node, const char* key) {
        if (!node) return nullptr;

        char lowerKey[257];
        strcpy(lowerKey, key);
        toLowerCase(lowerKey);

        char lowerNodeKey[257];
        strcpy(lowerNodeKey, node->key);
        toLowerCase(lowerNodeKey);

        if (strcmp(lowerKey, lowerNodeKey) == 0) {
            return node;
        } else if (strcmp(lowerKey, lowerNodeKey) < 0) {
            return find(node->left, key);
        } else {
            return find(node->right, key);
        }
    }

    const char* add(const char* word, uint64_t number) {
        char lowerWord[257];
        strcpy(lowerWord, word);
        toLowerCase(lowerWord);

        if (find(root, lowerWord)) return "Exist";

        root = insert(root, new Node(lowerWord, number, rand()));
        return "OK";
    }

    const char* del(const char* word) {
        if (!find(root, word)) return "NoSuchWord";
        char lowerWord[257];
        strcpy(lowerWord, word);
        toLowerCase(lowerWord);
        root = erase(root, lowerWord);
        return "OK";
    }

    const char* search(const char* word) {
        Node* result = find(root, word);
        static char output[280];  

        if (result) {
            snprintf(output, sizeof(output), "OK: %lu", result->value);
            return output;
        } else {
            return "NoSuchWord";
        }
    }

    Node* split(Node* current, const char* key, Node*& left, Node*& right) {
        if (!current) {
            left = right = nullptr;
        } else {
            char lowerKey[257];
            strcpy(lowerKey, key);
            toLowerCase(lowerKey);

            char lowerNodeKey[257];
            strcpy(lowerNodeKey, current->key);
            toLowerCase(lowerNodeKey);

            if (strcmp(lowerKey, lowerNodeKey) < 0) {
                split(current->left, key, left, current->left);
                right = current;
            } else {
                split(current->right, key, current->right, right);
                left = current;
            }
        }
        return current;
    }

    Node* merge(Node* left, Node* right) {
        if (!left || !right) return left ? left : right;
        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            return left;
        } else {
            right->left = merge(left, right->left);
            return right;
        }
    }

    Node* insert(Node* root, Node* node) {
        if (!root) return node;

        if (node->priority > root->priority) {
            split(root, node->key, node->left, node->right);
            return node;
        } else if (strcmp(node->key, root->key) < 0) {
            root->left = insert(root->left, node);
        } else {
            root->right = insert(root->right, node);
        }
        return root;
    }

    Node* erase(Node* root, const char* key) {
        if (!root) return nullptr;

        char lowerKey[257];
        strcpy(lowerKey, key);
        toLowerCase(lowerKey);

        char lowerRootKey[257];
        strcpy(lowerRootKey, root->key);
        toLowerCase(lowerRootKey);

        if (strcmp(lowerKey, lowerRootKey) == 0) {
            Node* result = merge(root->left, root->right);
            delete root;
            return result;
        } else if (strcmp(lowerKey, lowerRootKey) < 0) {
            root->left = erase(root->left, key);
        } else {
            root->right = erase(root->right, key);
        }
        return root;
    }
};

int main() {
    Treap dictionary;
    char command[512];

    while (std::cin.getline(command, 512)) {
        if (strncmp(command, "! Save ", 7) == 0) {
            const char* filename = command + 7;
            std::cout << dictionary.save(filename) << std::endl;
        } else if (strncmp(command, "! Load ", 7) == 0) {
            const char* filename = command + 7;
            std::cout << dictionary.load(filename) << std::endl;
        } else if (command[0] == '+') {
            char word[257] = "";
            uint64_t number = 0;
            sscanf(command, "+ %s %lu", word, &number);
            std::cout << dictionary.add(word, number) << std::endl;
        } else if (command[0] == '-') {
            char word[257] = "";;
            sscanf(command, "- %s", word);
            std::cout << dictionary.del(word) << std::endl;
        } else {
            char word[257] = "";;
            sscanf(command, "%s", word);
            std::cout << dictionary.search(word) << std::endl;
        }
    }

    return 0;
}
