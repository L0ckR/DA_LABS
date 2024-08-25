#include <iostream>
#include <vector>
#include <string>
#include <chrono>

// Функция для вычисления Z-функции
std::vector<int> computeZFunction(const std::string &s) {
    int n = s.size();
    std::vector<int> z(n, 0);
    int l = 0, r = 0;
    
    for (int i = 1; i < n; ++i) {
        if (i <= r)
            z[i] = std::min(r - i + 1, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]])
            ++z[i];
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    
    return z;
}

// Функция для поиска с использованием алгоритма Z-блоков
std::vector<int> zAlgorithmSearch(const std::string &text, const std::string &pattern) {
    std::string concat = pattern + "$" + text;
    int patternLength = pattern.size();
    std::vector<int> z = computeZFunction(concat);
    std::vector<int> positions;
    
    for (int i = 0; i < z.size(); ++i) {
        if (z[i] == patternLength) {
            positions.push_back(i - patternLength - 1);
        }
    }
    
    return positions;
}

// Функция для поиска с использованием std::string::find
std::vector<int> stdFindSearch(const std::string &text, const std::string &pattern) {
    std::vector<int> positions;
    size_t pos = text.find(pattern);
    
    while (pos != std::string::npos) {
        positions.push_back(pos);
        pos = text.find(pattern, pos + 1);
    }
    
    return positions;
}

// Функция для замера времени выполнения
template<typename Func>
void measureTime(Func func, const std::string &text, const std::string &pattern, const std::string &methodName) {
    auto start = std::chrono::high_resolution_clock::now();
    func(text, pattern);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsed = end - start;
    std::cout << methodName << " took " << elapsed.count() << " seconds." << std::endl;
}

int main() {
    std::string text, pattern;

    // Генерация большого текста путем повторения строки
    std::string baseText = "abacabadabacabae";
    int repeatCount = 1000;  // Количество повторений
    for (int i = 0; i < repeatCount; ++i) {
        text += baseText;
    }

    pattern = "ab";

    // Замер времени для алгоритма Z-блоков
    measureTime(zAlgorithmSearch, text, pattern, "Z-algorithm");

    // Замер времени для std::string::find
    measureTime(stdFindSearch, text, pattern, "std::string::find");

    return 0;
}
