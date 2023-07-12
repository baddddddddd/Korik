#ifndef STACK_HPP
#define STACK_HPP

#include "linked_list.hpp"

template <typename T>
class Stack {
private:
    LinkedList<T> list;

public:
    T& operator[](int index) {
        return list[index];
    }

    void push(const T value) {
        list.push_back(value);
    }

    T pop() {
        auto top = list.back();
        list.pop_back();
        return top;
    }

    T peek() {
        return list.back();
    }

    void print() {
        list.print();
    }
};

#endif // STACK_HPP