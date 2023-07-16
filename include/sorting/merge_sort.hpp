#ifndef MERGE_SORT_HPP
#define MERGE_SORT_HPP

#include "../data_structures/array.hpp"

template <typename T>
Array<T> _merge_sort_partition(Array<T>& arr, int low, int high) {
    if (low == high) {
        Array<T> sorted;
        sorted.push_back(arr[low]);
        return sorted;
    }

    int mid = (low + high) >> 1;

    auto left = _merge_sort_partition(arr, low, mid);
    auto right = _merge_sort_partition(arr, mid + 1, high);

    int left_pointer = 0;
    int right_pointer = 0;

    Array<T> sorted;
    sorted.reserve(high - low + 1);

    while (true) {
        if (left[left_pointer] < right[right_pointer]) {
            sorted.push_back(left[left_pointer]);
            left_pointer++;
        } else {
            sorted.push_back(right[right_pointer]);
            right_pointer++;
        }

        if (left_pointer == left.get_size()) {
            for (int i = right_pointer; i < right.get_size(); i++) {
                sorted.push_back(right[i]);
            }

            break;

        } else if (right_pointer == right.get_size()) {
            for (int i = left_pointer; i < left.get_size(); i++) {
                sorted.push_back(left[i]);
            }

            break;
        }
    }

    return sorted;
}

template <typename T>
void merge_sort(Array<T>& arr) {
    auto sorted = _merge_sort_partition(arr, 0, arr.get_size() - 1);

    for (int i = 0; i < arr.get_size(); i++) {
        arr[i] = sorted[i];
    }
}

#endif // HEAP_SORT_HPP