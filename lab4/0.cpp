#include <iostream>
#include <vector>
#include <string>

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

int main() {
    std::string text, pattern;
    
    std::cin >> text >> pattern;
    
    std::string concat = pattern + "$" + text;
    int patternLength = pattern.size();
    
    std::vector<int> z = computeZFunction(concat);
    
    for (int i = 0; i < z.size(); ++i) {
        if (z[i] == patternLength) {
            std::cout << (i - patternLength - 1) << std::endl;
        }
    }

    return 0;
}
