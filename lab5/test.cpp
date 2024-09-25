#include <chrono>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include "suffix_tree.hpp"

// Реализация алгоритма Бо Лэнга
int Booth(const std::string& s) {
    int n = s.size();
    std::string s2 = s + s;
    std::vector<int> f(2 * n, -1);
    int k = 0;
    for (int j = 1; j < 2 * n; ++j) {
        char sj = s2[j];
        int i = f[j - k - 1];
        while (i != -1 && sj != s2[k + i + 1]) {
            if (sj < s2[k + i + 1]) {
                k = j - i - 1;
            }
            i = f[i];
        }
        if (sj != s2[k + i + 1]) {
            if (sj < s2[k + i + 1]) {
                k = j;
            }
            f[j - k] = -1;
        } else {
            f[j - k] = i + 1;
        }
    }
    return k;
}

std::string GetBoothMinimalCyclicShift(const std::string& s) {
    int start_index = Booth(s);
    return s.substr(start_index, s.size());
}

// Функция для измерения времени выполнения
template<typename Func>
double MeasureTime(Func f) {  // Убираем аргумент строки из шаблона функции
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count();
}

// Функция для генерации случайной строки заданной длины
std::string GenerateRandomString(size_t length) {
    std::string chars = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += chars[rand() % chars.size()];
    }
    return result;
}

int main() {
    std::vector<size_t> string_lengths = {1000, 5000, 10000, 50000, 100000, 500000};  // Длины строк для тестирования
    std::cout << "Length, Suffix Tree Time (ms), Booth Algorithm Time (ms)\n";
    
    for (size_t length : string_lengths) {
        std::string s = GenerateRandomString(length);
        int n = s.length();
        s += s;  // Удвоим строку для проверки циклического сдвига
        
        // Тестируем суффиксное дерево
        SuffixTree suffixTree(s);
        double suffix_tree_time = MeasureTime([&]() {
            suffixTree.LexMinString(n);
        });
        
        // Тестируем алгоритм Бо Лэнга
        double booth_time = MeasureTime([&]() {
            GetBoothMinimalCyclicShift(s);
        });
        
        // Выводим результаты
        std::cout << length << ", " << suffix_tree_time << ", " << booth_time << "\n";
    }

    return 0;
}
