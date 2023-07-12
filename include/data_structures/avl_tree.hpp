#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include "queue.hpp"

template <typename T>
class AVLTree {
private:
    struct Node {
        int height = 0;

        T data;
        Node* parent;
        Node* left;
        Node* right;

        Node(const T value)
            : data(value), parent(NULL), left(NULL), right(NULL)
        {
        }

        int get_height() {
            return height;
        }
    };

    Node* root;

    void update_height(Node* node) {
        int left_height = (node->left) ? node->left->height : -1;
        int right_height = (node->right) ? node->right->height : -1;
        
        node->height = std::max(left_height, right_height) + 1;

        if (node->parent) {
            update_height(node->parent);
        }
    }

    int get_balance_factor(Node* node) {
        if (!node) {
            throw std::runtime_error("Cannot update null node");
        }

        int left_height = (node->left) ? node->left->height : -1;
        int right_height = (node->right) ? node->right->height : -1;

        node->height = std::max(left_height, right_height) + 1;

        int balance_factor = right_height - left_height;

        return balance_factor;
    }

    void insert_next(Node* node, T value) {
        if (node->data == value) {
            return;
        }

        auto& next_node = (value < node->data) ? node->left : node->right;

        if (!next_node) {
            next_node = new Node(value);
            next_node->parent = node;
            node->height = 1;
            return;
        }
        
        insert_next(next_node, value);

        int balance_factor = get_balance_factor(node);

        if (balance_factor < -1) {
            if (value > next_node->data) {
                left_rotate(next_node);
            }

            right_rotate(node);

        } else if (balance_factor > 1) {
            if (value < next_node->data) {
                right_rotate(next_node);
            }

            left_rotate(node);
        }
    }

    void left_rotate(Node* node) {
        if (!node) {
            throw std::runtime_error("Node is null");
        }

        if (!node->right) {
            throw std::runtime_error("Cannot left rotate: there is no right child");
        }

        auto x = node;
        auto y = node->right;

        if (x->parent) {
            y->parent = x->parent;

            auto& parent_link = (x->parent->left == x) ? x->parent->left : x->parent->right;
            parent_link = y;

        } else {
            y->parent = NULL;
            root = y;
        }

        x->right = y->left;
        if (x->right) {
            x->right->parent = x;
        }

        y->left = x;
        x->parent = y;

        update_height(x);
    }

    void right_rotate(Node* node) {
        if (!node) {
            throw std::runtime_error("Node is null");
        }

        if (!node->left) {
            throw std::runtime_error("Cannot right rotate: there is no left child");
        }

        auto x = node->left;
        auto y = node;

        if (y->parent) {
            x->parent = y->parent;

            auto& parent_link = (y->parent->left == y) ? y->parent->left : y->parent->right;
            parent_link = x;
        
        } else {
            x->parent = NULL;
            root = x;
        }

        y->left = x->right;
        if (y->left) {
            y->left->parent = y;
        }

        x->right = y;
        y->parent = x;

        update_height(y);
    }

    void remove_node(Node* node) {
        if (!node) {
            throw std::runtime_error("Node is null");
        }

        // case 1: leaf node
        // case 2: linked list
        // case 3: two children

        // case 3
        if (node->left && node->right) {
            auto target = successor(node);

            node->data = target->data;
            return remove_node(target);
        }

        // case 1
        if (!(node->left || node->right)) {
            if (node == root) {
                root = NULL;
                return;
            }

            auto& parent_link = (node->parent->left == node) ? node->parent->left : node->parent->right;
            parent_link = NULL;

            update_height(node->parent);

        } else {
            // case 2
            auto& child = (node->left) ? node->left : node->right;

            if (node == root) {
                root = child;
                root->parent = NULL;
                return;
            }

            auto& parent_link = (node->parent->left == node) ? node->parent->left : node->parent->right;

            parent_link = child;
            child->parent = node->parent;

            update_height(node->parent);
        }
    }

public:
    AVLTree()
        : root(NULL)
    {
    }

    ~AVLTree() {
        clear();
    }

    Node* get_root() const {
        return root;
    }

    void insert(const T value) {
        if (!root) {
            root = new Node(value);
            return;
        }

        insert_next(root, value);
    }

    Node* search(const T& value) {
        Node* current = root;

        while (current) {
            if (current->data == value) {
                return current;
            }

            current = (value < current->data) ? current->left : current->right;
        }

        return NULL;
    }

    void remove(const T& value) {
        auto node = search(value);
        remove_node(node);

        auto& parent_link = (node->parent->left == node) ? node->parent->left : node->parent->right;
        parent_link = NULL;

        auto current = node->parent;
    }

    Node* subtree_first(Node* node) {
        if (!node) {
            throw std::runtime_error("Node is null");
        }

        auto current = node;

        while (current->left) {
            current = current->left;
        }

        return current;
    }

    Node* subtree_last(const Node* node) {
        if (!node) {
            throw std::runtime_error("Node is null");
        }

        auto current = node;

        while (current->right) {
            current = current->right;
        }

        return current;
    }

    Node* successor(const Node* node) {
        if (!node) {
            throw std::runtime_error("Node is null");
        }

        if (node->right) {
            return subtree_first(node->right);
        }

        auto current = node;

        while (true) {
            if (!current->parent) {
                throw std::runtime_error("No successor found");
            }

            if (current->parent->left == current) {
                return current->parent;
            }

            current = current->parent;
        }
    }

    void print_subtree(const Node* node) {
        if (node->left) {
            print_subtree(node->left);
        }

        std::cout << node->data << " ";

        if (node->right) {
            print_subtree(node->right);
        }
    }

    void delete_subtree(Node* node) {
        if (!node) {
            return;
        }

        if (node->left) {
            delete_subtree(node->left);
        }

        if (node->right) {
            delete_subtree(node->right);
        }

        delete node;
    }

    void clear() {
        delete_subtree(root);
    }

    void print() {
        print_subtree(root);
        std::cout << std::endl;
    }

    void bfs() {
        Queue<Node*> to_visit;

        to_visit.enqueue(root);

        while (!to_visit.is_empty()) {
            auto node = to_visit.dequeue();
            std::cout << node->data << " ";

            if (node->left) {
                to_visit.enqueue(node->left);
            }

            if (node->right) {
                to_visit.enqueue(node->right);
            }
        }

        std::cout << std::endl;
    }
};

#endif // AVL_TREE_HPP