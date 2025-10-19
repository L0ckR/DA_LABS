#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

double calculateTriangleArea(double s1, double s2, double s3, double halfSum) 
{
    return sqrt(halfSum * (halfSum - s1) * (halfSum - s2) * (halfSum - s3));
}

int main() 
{
    int count;
    std::cin >> count;

    std::vector<int> elements(count);
    for (int i = 0; i < count; ++i) 
    {
        std::cin >> elements[i];
    }

    if (count < 3) 
    {
        std::cout << 0 << "\n";
        return 0;
    }

    std::sort(elements.rbegin(), elements.rend());

    int index = 0;
    std::cout << std::fixed << std::setprecision(3);

    double maxTriangleArea = -1.0;
    int foundIndex = -1;
    bool isPossible = false;

    while (index < count - 2) 
    {
        if (elements[index] < elements[index + 1] + elements[index + 2]) 
        {
            double side1 = (double)elements[index], side2 = (double)elements[index + 1], side3 = (double)elements[index + 2];
            double halfSum = (side1 + side2 + side3) / 2.0;

            double currentArea = calculateTriangleArea(side1, side2, side3, halfSum);
            
            if (currentArea > maxTriangleArea) 
            {
                maxTriangleArea = currentArea;
                foundIndex = index;
            }
            isPossible = true;
        }
        ++index;
    }

    if (isPossible) 
    {
        std::cout << maxTriangleArea << "\n";
        std::cout << elements[foundIndex + 2] << " " << elements[foundIndex + 1] << " " << elements[foundIndex] << "\n";
    } 
    else 
    {
        std::cout << 0 << "\n";
    }
}