#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "linked_list.hpp"

template <typename T>
class Queue {
private:
    LinkedList<T> list;

public:
    void enqueue(const T value) {
        list.push_back(value);
    }

    T dequeue() {
        auto front = list.front();
        list.pop_front();

        return front;
    }

    bool is_empty() {
        return list.empty();
    }
    
    void peek() {
        return list.front();
    }

};

#endif // QUEUE_HPP