#include <iostream>
#include <map>
#include <chrono>
#include <random>
#include "treap.h" // Включаем вашу реализацию Treap

int main() {
    // Массив с различными числами элементов для тестирования
    std::vector<int> testSizes = {1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000, 10000000};

    for (int numElements : testSizes) {
        std::map<std::string, uint64_t> stlMap;
        Treap customTreap;

        // Генерация случайных данных
        std::vector<std::string> keys(numElements);
        std::vector<uint64_t> values(numElements);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(1, 1000000000);

        for (int i = 0; i < numElements; ++i) {
            keys[i] = "key" + std::to_string(i);
            values[i] = dis(gen);
        }

        std::cout << "\nTesting with " << numElements << " elements:\n";

        // Тест производительности для std::map (STL)
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numElements; ++i) {
            stlMap[keys[i]] = values[i];
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> stlInsertTime = end - start;
        std::cout << "STL Map Insertion Time: " << stlInsertTime.count() << " seconds\n";

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numElements; ++i) {
            auto it = stlMap.find(keys[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> stlSearchTime = end - start;
        std::cout << "STL Map Search Time: " << stlSearchTime.count() << " seconds\n";

        // Тест производительности для пользовательской реализации Treap
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numElements; ++i) {
            customTreap.add(keys[i].c_str(), values[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> treapInsertTime = end - start;
        std::cout << "Custom Treap Insertion Time: " << treapInsertTime.count() << " seconds\n";

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numElements; ++i) {
            customTreap.search(keys[i].c_str());
        }
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> treapSearchTime = end - start;
        std::cout << "Custom Treap Search Time: " << treapSearchTime.count() << " seconds\n";
    }

    return 0;
}
