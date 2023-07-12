#ifndef ANSWERS_HPP
#define ANSWERS_HPP

#include <data_structures/stack.hpp>
#include <string>

class Answers {
private:
    Stack<std::string> answers;

public:
    Answers() {

    }

    ~Answers() {

    }

    std::string operator[](int index) {
        return answers[index];
    }

    void add_answer(std::string answer) {
        answers.push(answer);
    }

    void undo_answer() {
        answers.pop();
    }

    void print() {
        answers.print();
    }
};

#endif // ANSWERS_HPP