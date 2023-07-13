#ifndef EXAM_HPP
#define EXAM_HPP

#include "data_structures/array.hpp"

#include <string>

class Exam {
private:
    

public:
    std::string uploader;
    std::string title;
    std::string exam_code;
    Array<std::string> answer_key;

    Exam(std::string uploader, std::string title, std::string exam_code)
        : uploader(uploader), title(title), exam_code(exam_code)
    {
    }

    ~Exam() {

    }

    bool operator<(const Exam& other) const {
        return exam_code < other.exam_code;
        // return last_name < other.last_name
        //     && first_name < other.first_name
        //     && middle_name < other.middle_name;
    }

    bool operator>(const Exam& other) const {
        return exam_code > other.exam_code;
        // return last_name > other.last_name
        //     && first_name > other.first_name
        //     && middle_name > other.middle_name;
    }

    bool operator<=(const Exam& other) const {
        return exam_code <= other.exam_code;
        // return last_name <= other.last_name
        //     && first_name <= other.first_name
        //     && middle_name <= other.middle_name;
    }

    bool operator>=(const Exam& other) const {
        return exam_code >= other.exam_code;
        // return last_name >= other.last_name
        //     && first_name >= other.first_name
        //     && middle_name >= other.middle_name;
    }

    bool operator==(const Exam& other) const {
        return exam_code == other.exam_code;

        // bool has_same_exam_code = exam_code == other.exam_code;
        // bool has_same_email = email_address == other.email_address;
        // bool has_same_name = last_name == other.last_name && first_name == other.first_name && middle_name == other.middle_name;
        // 
        // return has_same_exam_code || has_same_email || has_same_name;
    }

    bool operator!=(const Exam& other) const {
        return !(*this == other);
    } 
};

#endif // EXAM_HPP