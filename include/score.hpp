#ifndef SCORE_HPP
#define SCORE_HPP

#include <data_structures/array.hpp>
#include <string>

class Score {
private:

public:
    std::string uploader;
    std::string exam_code;
    Array<std::string> answers;

    Score(std::string uploader, std::string exam_code)
        : uploader(uploader), exam_code(exam_code)
    {
    }

    bool operator<(const Score& other) const {
        return exam_code < other.exam_code;
        // return last_name < other.last_name
        //     && first_name < other.first_name
        //     && middle_name < other.middle_name;
    }

    bool operator>(const Score& other) const {
        return exam_code > other.exam_code;
        // return last_name > other.last_name
        //     && first_name > other.first_name
        //     && middle_name > other.middle_name;
    }

    bool operator<=(const Score& other) const {
        return exam_code <= other.exam_code;
        // return last_name <= other.last_name
        //     && first_name <= other.first_name
        //     && middle_name <= other.middle_name;
    }

    bool operator>=(const Score& other) const {
        return exam_code >= other.exam_code;
        // return last_name >= other.last_name
        //     && first_name >= other.first_name
        //     && middle_name >= other.middle_name;
    }

    bool operator==(const Score& other) const {
        return exam_code == other.exam_code;

        // bool has_same_exam_code = exam_code == other.exam_code;
        // bool has_same_email = email_address == other.email_address;
        // bool has_same_name = last_name == other.last_name && first_name == other.first_name && middle_name == other.middle_name;
        // 
        // return has_same_exam_code || has_same_email || has_same_name;
    }

    bool operator!=(const Score& other) const {
        return !(*this == other);
    } 
};

#endif // SCORE_HPP