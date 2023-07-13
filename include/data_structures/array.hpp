#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <string>

template <typename T>
class Array {
private:
    int size;
    int capacity;
    T* arr;

    void grow() {
        capacity *= 2;

        T* old_arr = arr;
        arr = new T[capacity];

        for (int i = 0; i < size; i++) {
            arr[i] = old_arr[i];
        }

        delete[] old_arr;
    }

public:
    Array() {
        size = 0;
        capacity = 2;
        arr = new T[capacity];
    }

    ~Array() {
        delete[] arr;
    }

    Array(const Array& other) {
        size = other.size;
        capacity = other.capacity;

        arr = new T[capacity];

        for (int i = 0; i < size; i++) {
            arr[i] = other.arr[i];
        }
    }

    int get_size() {
        return size;
    }

    T& operator[](int index) {
        return arr[index];
    }

    void push_back(T value) {
        if (size == capacity) {
            grow();
        }

        arr[size] = value;
        size++;
    }

    T pop_back() {
        if (size == 0) {
            throw std::runtime_error("Cannot pop from an empty array");
        }

        size--;
        return arr[size];
    }

    void insert(int index, T value) {
        if (index < 0 || index > size) {
            throw std::runtime_error("Cannot insert elements beyond the bounds of the array");
        }

        if (size == capacity) {
            grow();
        }

        for (int i = size; i > index; i--) {
            arr[i] = arr[i - 1];
        }

        arr[index] = value;
        size++;
    }

    void erase(int index) {
        if (index < 0 || index >= size) {
            throw std::runtime_error("Cannot delete elements beyond the bounds of the array");
        }

        size--;

        for (int i = index; i < size; i++) {
            arr[i] = arr[i + 1];
        }
    }

    void erase_by_value(T value, bool include_duplicates = false) {
        for (int i = 0; i < size; ) {
            if (arr[i] == value) {
                erase(i);

                if (!include_duplicates) {
                    return;
                }

            } else {
                i++;
            }
        }
    }

    int get_index(T value) {
        for (int i = 0; i < size; i++) {
            if (arr[i] == value) {
                return i;
            }
        }

        return -1;
    }
};

#endif // ARRAY_HPP