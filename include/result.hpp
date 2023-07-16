#ifndef RESULT_HPP
#define RESULT_HPP

#include "exam.hpp"
#include "score.hpp"
#include "user.hpp"

class Result {
public:
    User* user;
    Score* score;
    Exam* exam;
    int grade = 0;

    Result() {

    }
    
    Result(User* user, Exam* exam, Score* score)
        : user(user), exam(exam), score(score)
    {
        if (exam->exam_code != score->exam_code) {
            throw std::runtime_error("Invalid result set");
        }

        for (int i = 0; i < exam->answer_key.get_size(); i++) {
            if (exam->answer_key[i] == score->answers[i]) {
                grade++;
            }
        }
    }

    bool operator<(const Result& other) const {
        return grade < other.grade;
    }

    bool operator>(const Result& other) const {
        return grade > other.grade;
    }

    bool operator<=(const Result& other) const {
        return grade <= other.grade;
    }

    bool operator>=(const Result& other) const {
        return grade >= other.grade;
    }

    bool operator==(const Result& other) const {
        return grade == other.grade;
    }

    bool operator!=(const Result& other) const {
        return !(*this == other);
    }     
};

#endif // RESULT_HPP