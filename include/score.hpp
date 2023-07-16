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

    Score(const Score& other)
        : uploader(other.uploader), exam_code(other.exam_code), answers(other.answers)
    {
    }

    bool operator<(const Score& other) const {
        return uploader < other.uploader;
        // return last_name < other.last_name
        //     && first_name < other.first_name
        //     && middle_name < other.middle_name;
    }

    bool operator>(const Score& other) const {
        return uploader > other.uploader;
        // return last_name > other.last_name
        //     && first_name > other.first_name
        //     && middle_name > other.middle_name;
    }

    bool operator<=(const Score& other) const {
        return uploader <= other.uploader;
        // return last_name <= other.last_name
        //     && first_name <= other.first_name
        //     && middle_name <= other.middle_name;
    }

    bool operator>=(const Score& other) const {
        return uploader >= other.uploader;
        // return last_name >= other.last_name
        //     && first_name >= other.first_name
        //     && middle_name >= other.middle_name;
    }

    bool operator==(const Score& other) const {
        return uploader == other.uploader;

        // bool has_same_exam_code = exam_code == other.exam_code;
        // bool has_same_email = email_address == other.email_address;
        // bool has_same_name = last_name == other.last_name && first_name == other.first_name && middle_name == other.middle_name;
        // 
        // return has_same_exam_code || has_same_email || has_same_name;
    }

    bool operator!=(const Score& other) const {
        return !(*this == other);
    } 

    void print_score() {
        std::cout << "Uploader: " << uploader << std::endl;
        std::cout << "Exam code: " << exam_code << std::endl;
        std::cout << "Answers: " << std::endl;

        for (int i = 0; i < answers.get_size(); i++) {
            std::cout << (i + 1) << ". " << answers[i] << std::endl; 
        }
    }
};

#endif // SCORE_HPP