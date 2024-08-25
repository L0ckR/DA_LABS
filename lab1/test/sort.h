#include <iostream>
#include <cstdint>

template <typename T>

class TVector {
    protected:
        T* arr;
        size_t capacity, current;
    
    public:
        TVector()
        {
            arr = new T[1];
            capacity = 1;
            current = 0;
        }

        TVector(size_t initializedSize)
        {
            arr = new T[initializedSize];
            capacity = initializedSize;
            current = initializedSize;
        }

        TVector(size_t initializedSize, T value)
        {
            arr = new T[initializedSize];
            capacity = initializedSize;
            current = initializedSize;
            for (size_t i = 0; i < initializedSize; ++i){
                arr[i] = value;
            }
        }

        ~TVector() { 
            delete[] arr; 
        }

        void PushBack(T data)
        {
            if (current == capacity) {
                T* temp = new T[2 * capacity];
                for (size_t i = 0; i < capacity; ++i) {
                    temp[i] = arr[i];
                }
                delete[] arr;
                capacity *= 2;
                arr = temp;
            }
            arr[current] = data;
            ++current;
        }

        size_t GetSize() { 
            return current; 
        }

        size_t GetCapacity() {
            return capacity;
        }

        void Print()
        {
            for (size_t i = 0; i < current; ++i) {
                std::cout << arr[i] << "\n";
            }
        }
        
        TVector<T> & operator=(const TVector &other)
        {
            delete[] arr;
            current = other.current;
            capacity = other.capacity;
            arr = new T [current];
            for (size_t i = 0; i < current; ++i){
                arr[i] = other.arr[i];
            }
            return *this;
        }

        T &operator[](size_t idx) {
            if (idx > current) {
                throw std::out_of_range("Vector index out of range");
            }
            return arr[idx];
        }

        const T &operator[](size_t idx) const {
            if (idx > current) {
                throw std::out_of_range("Vector index out of range");
            }
            return arr[idx];
        }
};

struct TItem
{
    uint16_t key = 0;
    uint64_t value = 0;

    friend std::ostream& operator<<(std::ostream& os, TItem const& item)
    {
        return os << item.key << "\t" << item.value;
    }
};


TVector<TItem> CountSort(TVector<TItem> &vec)
{
 
    size_t size = vec.GetSize();
 
    uint16_t maximum = 0;
 
    for (size_t i = 0; i < size; ++i){
        maximum = std::max(maximum, vec[i].key);
    }
 
    TVector<size_t> countArray(maximum + 1, 0);
 
    for (size_t i = 0; i < size; ++i){
        ++countArray[vec[i].key];
    }
 
    for (int i = 1; i <= maximum; ++i){
        countArray[i] += countArray[i - 1];
    }
 
    TVector<TItem> outputArray(size);
 
    for (size_t i = size; i > 0; --i){
        outputArray[countArray[vec[i - 1].key] - 1] = vec[i - 1];
        --countArray[vec[i - 1].key];
    }
    return outputArray;
}

