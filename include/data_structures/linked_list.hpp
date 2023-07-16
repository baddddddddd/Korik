#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <iostream>

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(value), next(NULL) {}
    };

    Node* head;
    Node* tail;

public:
    LinkedList() : head(NULL), tail(NULL) {}

    ~LinkedList() {
        clear();
    }

    LinkedList(const LinkedList& other) {
        head = NULL;
        tail = NULL;

        Node* currNode = other.head;

        while (currNode != NULL) {
            Node* new_node = new Node(currNode->data);

            if (head == NULL) {
                head = new_node;

            } else {
                tail->next = new_node;
            }

            tail = new_node;
            
            currNode = currNode->next;
        }
    }

    T front() {
        return head->data;
    }

    T back() {
        return tail->data;
    }

    T& operator[](int index) {
        Node* current = head;

        for (int i = 0; i < index; i++) {
            if (!current) {
                throw std::runtime_error("Index out of range.");
            }

            current = current->next;
        }

        if (!current) {
            throw std::runtime_error("Index out of range.");
        }

        return current->data;
    }

    void push_back(const T value) {
        Node* new_node = new Node(value);

        if (head == NULL) {
            head = new_node;

        } else {
            tail->next = new_node;
        }

        tail = new_node;
    }

    void push_front(const T value) {
        Node* new_node = new Node(value);

        if (head == NULL) {
            tail = new_node;

        } else {
            new_node->next = head;
        }

        head = new_node;
    }

    void insert(const int index, const T value) {
        Node* current = head;
        Node* prev = NULL;

        for (int i = 0; i < index; i++) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            return push_front(value);

        } else if (current == NULL) {
            return push_back(value);
        }

        Node* new_node = new Node(value);
        prev->next = new_node;
        new_node->next = current;
    }

    void pop_back() {
        if (tail == NULL) {
            throw std::runtime_error("Cannot delete from an empty linked list.");
        }

        Node* old_tail = tail;

        if (head == tail) {
            head = NULL;
            tail = NULL;

        } else {
            Node* current = head;

            while (current->next != tail) {
                current = current->next;
            }

            tail = current;
            tail->next = NULL;
        }

        delete old_tail;
    }

    void pop_front() {
        if (head == NULL) {
            throw std::runtime_error("Cannot delete from an empty linked list.");
        }

        Node* old_head = head;

        if (head == tail) {
            head = NULL;
            tail = NULL;

        } else {
            head = head->next;
        }
        
        delete old_head;
    }

    void erase(int index) {
        Node* current = head;
        Node* prev = NULL;

        for (int i = 0; i < index; i++) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            return pop_front();

        } else if (current == NULL) {
            return pop_back();
        }

        prev->next = current->next;
        delete current;
    }

    void remove(const T value) {
        Node* currNode = head;
        Node* prevNode = NULL;
        while (currNode != NULL) {
            if (currNode->data == value) {
                if (prevNode == NULL) {
                    head = currNode->next;
                } else {
                    prevNode->next = currNode->next;
                }
                if (currNode == tail) {
                    tail = prevNode;
                }
                delete currNode;
                return;
            }
            prevNode = currNode;
            currNode = currNode->next;
        }
    }

    bool empty() const {
        return head == NULL;
    }

    void clear() {
        Node* current = head;

        while (current) {
            Node* next_node = current->next;

            delete current;
            
            current = next_node;
        }
        
        head = NULL;
        tail = NULL;
    }

    void print() const {
        Node* currNode = head;
        while (currNode != NULL) {
            std::cout << currNode->data << " ";
            currNode = currNode->next;
        }
        std::cout << std::endl;
    }
};

#endif // LINKED_LIST_HPP
