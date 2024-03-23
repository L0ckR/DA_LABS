#include <chrono>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "1-3.cpp"

struct
{
    bool operator()(TItem a, TItem b) const { return a.key < b.key; }
}
customLess;


double getAvgTimeForCount(TVector<TItem> & vec) {
    constexpr int runsCount = 5;
    
    double avg = 0;

    for(int i = 0; i < runsCount; ++i) {
        auto begin = std::chrono::high_resolution_clock::now();
        CountSort(vec);
        auto end = std::chrono::high_resolution_clock::now();
        avg += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }

    return avg / runsCount;
};

double getAvgTimeForSTD(std::vector<TItem> & vec) {
    constexpr int runsCount = 5;
    double avg = 0;
    int runsCaaount = 5;
    for(int i = 0; i < runsCount; ++i) {  
        std::vector<TItem> vecCopy = vec;
        auto begin = std::chrono::high_resolution_clock::now();
        std::sort(vecCopy.begin(), vecCopy.end(), customLess);
        auto end = std::chrono::high_resolution_clock::now();
        avg += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }

    return avg / runsCount;
};

int main(){
    std::cout.precision(14);
    std::cout << std::fixed;
    TVector<TItem> vec1;
    std::vector<TItem> vec2;
    TItem input;
    while(std::cin >> input.key){
        std::cin >> input.value;
        vec1.PushBack(input);
        vec2.push_back(input);
    }
    std::cout <<  getAvgTimeForCount(vec1) << std::endl;
    std::cout << getAvgTimeForSTD(vec2) << std::endl;

}