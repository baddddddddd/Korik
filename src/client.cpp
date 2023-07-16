#include <data_structures/avl_tree.hpp>
#include <user.hpp>
#include <answers.hpp>
#include <exam.hpp>
#include <score.hpp>

#include <cpprest/http_client.h>
#include <cpprest/json.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#define CLEAR_SCREEN "cls"
#define PAUSE "pause"

#define SERVER_URL std::string("http://localhost:8080")

#define TO_JSON_STRING(s) json::value::string(utility::conversions::to_string_t(s))

using namespace web;
using namespace web::http;
using namespace web::http::client;

User* user = NULL;

json::value send_post_request(const utility::string_t& endpoint, const json::value& post_data) {
    uri_builder builder(endpoint);
    http_client client(builder.to_string());

    http_request request(methods::POST);
    request.headers().set_content_type(U("application/json"));
    request.set_body(post_data);

    return client.request(request)
        .then([](http_response response) {
            return response.extract_json();
        })
        .get();
}

std::string get_value(const json::value& dict, const std::string& key) {
    const std::wstring& value_w = dict.at(utility::conversions::to_string_t(key)).as_string();
    std::string value(value_w.begin(), value_w.end());

    return value;
}

std::string get_line(std::string prompt = "Answer: ") {
    std::cout << prompt;

    std::string answer;
    std::getline(std::cin, answer);
    return answer;
}

void show_invalid_answer() {
    std::cout << "\nInvalid answer. Please try again.\n";
    system(PAUSE);
}

std::string hash_password(std::string password) {
    std::hash<std::string> hasher;
    size_t hash = hasher(password);

    std::stringstream ss;
    ss << std::hex << hash;

    return ss.str();
}

bool verify_login(std::string username, std::string password) {
    auto hashed_pw = hash_password(password);
    
    json::value body;
    body[U("username")] = TO_JSON_STRING(username);
    body[U("pw_hash")] = TO_JSON_STRING(hashed_pw);

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/login"), body);

    if (response.has_field(U("success"))) {
        auto is_success = get_value(response, "success");
        
        if (is_success == "true") {
            if (user != NULL) {
                delete user;
            }

            bool is_instructor = false;

            if (response.has_field(U("is_instructor"))) {
                is_instructor = true;
            }

            user = new User(
                "0",
                username,
                hashed_pw,
                get_value(response, "email_address"),
                get_value(response, "last_name"),
                get_value(response, "first_name"),
                get_value(response, "middle_name"),
                is_instructor
            );

            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

bool create_user(User& new_user) {
    json::value body;
    body[U("username")] = TO_JSON_STRING(new_user.get_username());
    body[U("email_address")] = TO_JSON_STRING(new_user.get_email_address());
    body[U("pw_hash")] = TO_JSON_STRING(new_user.get_password_hash());
    body[U("last_name")] = TO_JSON_STRING(new_user.get_last_name());
    body[U("first_name")] = TO_JSON_STRING(new_user.get_first_name());
    body[U("middle_name")] = TO_JSON_STRING(new_user.get_middle_name());

    if (new_user.is_instructor) {
        body[U("is_instructor")] = TO_JSON_STRING("true");
    }
    
    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/register"), body);

    if (response.has_field(U("success"))) {
        auto is_success = get_value(response, "success");
        return is_success == "true";
    
    } else {
        return false;
    }
}

int check_exam_code(std::string code) {
    json::value body;
    body[U("code")] = TO_JSON_STRING(code);

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/check_exam"), body);

    if (response.has_field(U("success"))) {
        if (get_value(response, "success") == "true") {
            auto number_of_items = response.at(U("number_of_items")).as_number().to_int32();
            auto title = get_value(response, "title");
            return number_of_items;

        } else {
            return 0;
        }

    } else {
        return 0;
    }    
}

Answers* input_exam(int number_of_items, std::string title) {
    system(CLEAR_SCREEN);

    int current_number = 1;

    Answers* answers = new Answers;

    std::cout << "======= TAKING EXAM =======\n";
    std::cout << "Title: " << title << std::endl;

    while (current_number <= number_of_items) {
        auto answer = get_line(std::to_string(current_number) + ". ");

        if (answer == "-") {
            if (current_number > 1) {
                answers->undo_answer();
                current_number--;
            }

            system(CLEAR_SCREEN);

            std::cout << "======= TAKING EXAM =======\n";
            std::cout << "Title: " << title << std::endl;

            for (int i = 0; i < current_number - 1; i++) {
                std::cout << (i + 1) << ". " << (*answers)[i] << std::endl;
            }

        } else {
            answers->add_answer(answer);
            current_number++;
        }
    }

    return answers;
}

void upload_score(Score& score) {
    json::value body;
    body[U("uploader")] = TO_JSON_STRING(score.uploader);
    body[U("code")] = TO_JSON_STRING(score.exam_code);

    json::value answers;

    for (int i = 0; i < score.answers.get_size(); i++) {
        answers[i] = TO_JSON_STRING(score.answers[i]);
    }

    body[U("answers")] = answers;

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/upload_score"), body);

    if (response.has_field(U("success")) && get_value(response, "success") == "true") {
        std::cout << "\nSuccessfully uploaded exam\n";
        system(PAUSE);

    } else {
        std::cout << "\nFailed to upload exam. You may have already taken the exam.\n"; 
        system(PAUSE);
    }
}

void start_exam() {
    system(CLEAR_SCREEN);

    std::cout << "======= STARTING EXAM ========\n";
    
    auto code = get_line("Exam code: ");

    int number_of_items;
    std::string title;

    json::value body;
    body[U("code")] = TO_JSON_STRING(code);

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/check_exam"), body);

    if (response.has_field(U("success")) && get_value(response, "success") == "true") {
        number_of_items = response.at(U("number_of_items")).as_number().to_int32();
        title = get_value(response, "title");

    } else {
        std::cout << "\nInvalid exam code. Please try again...\n";
        system(PAUSE);
        return start_exam();
    }   

    auto answers = input_exam(number_of_items, title);

    Score score(user->get_username(), code);

    for (int i = 0; i < number_of_items; i++) {
        score.answers.push_back((*answers)[i]);
    }

    upload_score(score);

    delete answers;
}

void show_exam_history() {
    system(CLEAR_SCREEN);

    std::cout << "======= EXAM HISTORY =======\n";

    json::value body;
    body[U("username")] = TO_JSON_STRING(user->get_username());
    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/get_exams"), body);

    if (response.has_field(U("count"))) {
        int count = response.at(U("count")).as_integer();

        if (count > 0) {
            auto scores = response.at(U("scores")).as_array();

            int counter = 1;
            for (auto& score : scores) {
                auto code = get_value(score, "code");
                auto title = get_value(score, "title");
                auto grade = score.at(U("grade")).as_integer();
                auto item_count = score.at(U("item_count")).as_integer();

                std::cout << counter << ". " << title << " (" << code << ")" << " - " << grade << "/" << item_count << std::endl;

                counter++;
            }

            system(PAUSE);

        } else {
            std::cout << "\nYou haven't taken any exams yet.\n";
            system(PAUSE);
        }

    } else {
        std::cout << "\nSomething went wrong\n";
        system(PAUSE);
    }
}

Exam create_exam(std::string title, std::string code, int number_of_items) {
    system(CLEAR_SCREEN);

    Exam exam(user->get_username(), title, code);

    std::cout << "======= ANSWER KEY CREATION =======\n";
    std::cout << "Exam Title: " << title << std::endl;
    std::cout << "Exam Code: " << code << std::endl;
    std::cout << "Answer Key: " << std::endl;

    int current_number = 1;

    while (current_number <= number_of_items) {
        std::cout << current_number << ". ";

        auto answer = get_line("");

        exam.answer_key.push_back(answer);

        current_number++;
    }

    return exam;
}

void show_create_exam_ui() {
    system(CLEAR_SCREEN);

    std::cout << "======= EXAM CREATION =======\n";
    
    auto title = get_line("Title: ");
    auto code = get_line("Exam entry code: ");
    int number_of_items;

    while (true) {
        auto answer = get_line("Number of items: ");

        try {
            number_of_items = std::stoi(answer);

            if (number_of_items <= 0) {
                throw std::exception();
            }

            break;

        } catch (std::exception& e) {
            std::cout << "Invalid number. Please try again...\n";
            system(PAUSE);
            std::cout << std::endl;
        }
    }

    auto exam = create_exam(title, code, number_of_items);

    json::value answer_key;

    for (int i = 0; i < number_of_items; i++) {        
        answer_key[i] = TO_JSON_STRING(exam.answer_key[i]);
    }

    json::value body;
    body[U("uploader")] = TO_JSON_STRING(user->get_username());
    body[U("title")] = TO_JSON_STRING(title);
    body[U("code")] = TO_JSON_STRING(code);
    body[U("answer_key")] = answer_key;

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/create_exam"), body);

    if (response.has_field(U("success"))) {
        if (get_value(response, "success") == "true") {
            std::cout << "\nSuccessfully uploaded exam with code: \n";
            std::cout << code << std::endl << std::endl;
            system(PAUSE);
        } else {
            std::cout << "\nFailed to create exam. Exam code may be taken already...\n";
            system(PAUSE);
        }

    } else {
        std::cout << "\nFailed to create exam. Exam code may be taken already...\n";
        system(PAUSE);
    }
}

Score to_score(json::value& json_value) {
    auto uploader = get_value(json_value, "uploader");
    auto code = get_value(json_value, "code");
    auto answers = json_value.at(U("answers")).as_array();

    Score score(uploader, code);

    for (auto& answer : answers) {
        auto answer_w = answer.as_string();
        auto answer_string = std::string(answer_w.begin(), answer_w.end());
        score.answers.push_back(answer_string);
    }

    return score;
}

Exam to_exam(json::value& json_value) {
    auto uploader = get_value(json_value, "uploader");
    auto code = get_value(json_value, "code");
    auto title = get_value(json_value, "title");
    
    Exam exam(uploader, title, code);

    auto answer_key = json_value.at(U("answer_key")).as_array();
    for (auto& answer : answer_key) {
        auto answer_w = answer.as_string();
        std::string answer_string(answer_w.begin(), answer_w.end());
        exam.answer_key.push_back(answer_string);
    }

    if (json_value.has_field(U("scores"))) {
        auto scores = json_value.at(U("scores")).as_array();

        for (auto& score : scores) {
            exam.scores.insert(to_score(score));
            exam.score_count++;
        }
    }

    return exam;
}

void show_rankings(std::string code) {
    system(CLEAR_SCREEN);

    json::value body;
    body[U("username")] = TO_JSON_STRING(user->get_username());
    body[U("code")] = TO_JSON_STRING(code);
    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/view_rankings"), body);

    // TKKKK

    system(PAUSE);
}

void show_item_analysis(std::string code) {
    system(CLEAR_SCREEN);

    json::value body;
    body[U("username")] = TO_JSON_STRING(user->get_username());
    body[U("code")] = TO_JSON_STRING(code);
    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/view_exam"), body);

    auto exam_json = response.at(U("exam")); 
    Exam& exam = to_exam(exam_json);

    std::cout << "======= EXAM ITEM ANALYSIS =======\n";
    std::cout << "Title: " << exam.title << std::endl;
    std::cout << "Code: " << exam.exam_code << std::endl;
    
    if (!exam.scores.get_root()) {
        std::cout << "\nNo one has taken this exam yet.\n";
        system(PAUSE);
        return;
    }

    AVLTree<Score>::Node* current = NULL;
    
    int* item_correct = new int[exam.answer_key.get_size()];

    for (int i = 0; i < exam.answer_key.get_size(); i++) {
        item_correct[i] = 0;
    }

    int score_sum = 0;

    for (int i = 0; i < exam.score_count; i++) {
        if (!current) {
            current = exam.scores.subtree_first(exam.scores.get_root());
        } else {
            current = exam.scores.successor(current);
        }

        Score& score = current->data;

        int grade = 0;
        for (int j = 0; j < exam.answer_key.get_size(); j++) {
            if (exam.answer_key[j] == score.answers[j]) {
                grade++;
                item_correct[j]++;
            }
        }

        score_sum += grade;
    }

    float average = (float)score_sum / exam.score_count;

    std::cout << "Mean Score: " << average << std::endl;    
    std::cout << "\nNumber of correct answers per item: " << std::endl;

    for (int i = 0; i < exam.answer_key.get_size(); i++) {
        float percentage = ((float)item_correct[i] / exam.score_count) * 100;
        std::cout << (i + 1) << ". " << item_correct[i] << "/" << exam.score_count << " = " << percentage << "%\n";
    }

    std::cout << std::endl;
    system(PAUSE);

    delete[] item_correct;

    return show_rankings(code);
}

void show_view_exams_ui() {
    system(CLEAR_SCREEN);

    std::cout << "======= VIEW EXAMS =======\n";
    
    json::value body;
    body[U("username")] = TO_JSON_STRING(user->get_username());

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/my_exams"), body);
    
    if (response.has_field(U("none"))) {
        std::cout << "\nYou have yet to create an exam.\n\n";
        system(PAUSE);
        return;
    }

    auto exam_count = response.at(U("exam_count")).as_integer();
    auto exams = response.at(U("exams")).as_array();

    std::cout << "\nYou have created a total of " << exam_count << " exams\n";

    int counter = 1;
    for (auto& exam_json : exams) {
        auto title = get_value(exam_json, "title");
        auto code = get_value(exam_json, "code");

        std::cout << counter << ". " << title << " (" << code << ")\n";
        
        counter++;
    }

    int exam_number;
    
    while (true) {
        std::string answer = get_line("\nView Exam No.: ");
        
        try {
            exam_number = std::stoi(answer);

            if (exam_number < 1 || exam_number > exam_count) {
                throw std::exception();
            }

            break;

        } catch (std::exception& e) {
            
            std::cout << "\nInvalid number. Please try again...\n";
            system(PAUSE);
        }
    }

    auto code = get_value(exams[exam_number - 1], "code");
    return show_item_analysis(code);
}

void show_delete_exam_ui() {
    system(CLEAR_SCREEN);
    // TK
}

void show_instructor_dashboard() {
    system(CLEAR_SCREEN);

    std::cout << "======= INSTRUCTOR DASHBOARD =======\n";
    std::cout << "Logged in as: " << user->get_last_name() << ", " << user->get_first_name() << " " << user->get_middle_name() << std::endl;
    
    std::cout << std::endl;
    std::cout << "[1] Create exam\n";
    std::cout << "[2] View exams\n";
    std::cout << "[3] Delete exam\n";
    std::cout << "[4] Logout\n";

    auto answer = get_line("\nAnswer: ");

    if (answer == "1") {
        show_create_exam_ui();

    } else if (answer == "2") {
        show_view_exams_ui();

    } else if (answer == "3") {
        show_delete_exam_ui();

    } else if (answer == "4") {
        return;

    } else {
        show_invalid_answer();
    }

    return show_instructor_dashboard();
}

void show_exam_search_ui() {
    system(CLEAR_SCREEN);

    std::cout << "======= SEARCH EXAM BY CODE =======\n";
    auto code = get_line("Exam code: ");

    json::value body;
    body[U("username")] = TO_JSON_STRING(user->get_username());
    body[U("code")] = TO_JSON_STRING(code);

    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/search_exam_with_code"), body);

    if (response.has_field(U("none"))) {
        std::cout << "\nExam does not exist.\n";

        system(PAUSE);
        return;
    }

    if (response.has_field(U("untaken"))) {
        auto title = get_value(response, "title");
        auto item_count = response.at(U("item_count")).as_integer();
        auto score_count = response.at(U("score_count")).as_integer();

        std::cout << "\nExam found: " << title << std::endl;
        std::cout << "Number of items: " << item_count << std::endl;
        std::cout << "Number of students that took this exam: " << score_count << std::endl << std::endl;

        system(PAUSE);
        return;
    }

    system(CLEAR_SCREEN);

    auto title = get_value(response, "title");
    auto item_count = response.at(U("item_count")).as_integer();
    auto score_count = response.at(U("score_count")).as_integer();

    auto answers = response.at(U("answers")).as_array();
    auto answer_key = response.at(U("answer_key")).as_array();

    int grade = 0;
    for (int i = 0; i < item_count; i++) {
        auto answer_w = answers[i].as_string();
        std::string answer_str(answer_w.begin(), answer_w.end());

        auto answer_key_w = answer_key[i].as_string();
        std::string answer_key_str(answer_key_w.begin(), answer_key_w.end());

        if (answer_str == answer_key_str) {
            grade++;
        }
    }

    std::cout << "======= EXAM RESULT =======\n";
    std::cout << "Title: " << title << std::endl;
    std::cout << "Exam code: " << code << std::endl;
    std::cout << "Score: " << grade << "/" << item_count << std::endl;
    std::cout << "Answers: \n"; 

    for (int i = 0; i < item_count; i++) {
        auto answer_w = answers[i].as_string();
        std::string answer_str(answer_w.begin(), answer_w.end());

        auto answer_key_w = answer_key[i].as_string();
        std::string answer_key_str(answer_key_w.begin(), answer_key_w.end());

        std::cout << (i + 1) << ". " << answer_str;
        if (answer_str != answer_key_str) {
            std::cout << " -> " << answer_key_str;
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
    system(PAUSE);
}

void show_student_dashboard() {
    system(CLEAR_SCREEN);

    std::cout << "======= STUDENT DASHBOARD =======\n";
    std::cout << "Logged in as: " << user->get_last_name() << ", " << user->get_first_name() << " " << user->get_middle_name() << std::endl;
    
    std::cout << std::endl;
    std::cout << "[1] Enter exam code\n";
    std::cout << "[2] View exam history\n";
    std::cout << "[3] Search exam result with code\n";
    std::cout << "[4] Logout\n";

    auto answer = get_line("\nAnswer: ");

    if (answer == "1") {
        start_exam();

    } else if (answer == "2") {
        show_exam_history();

    } else if (answer == "3") {
        show_exam_search_ui();

    } else if (answer == "4") {
        return;

    } else {
        show_invalid_answer();
    }
 
    return show_student_dashboard();
}

void show_login_ui() {
    system(CLEAR_SCREEN);

    std::cout << "Please enter your login credentials\n";

    auto username = get_line("Username: ");
    auto password = get_line("Password: ");

    if (verify_login(username, password)) {
        if (user->is_instructor) {
            return show_instructor_dashboard();

        } else {
            return show_student_dashboard();
        }

    } else {
        std::cout << "\nIncorrect username or password. Please try again...\n";
        system(PAUSE);
        return;
    }
}

void show_registration_ui() {
    system(CLEAR_SCREEN);

    std::cout << "Which one describes you?\n";
    std::cout << "[1] Student\n";
    std::cout << "[2] Instructor\n";

    auto answer = get_line("\nAnswer: ");
    bool is_instructor;

    if (answer == "1") {
        is_instructor = false;

    } else if (answer == "2") {
        is_instructor = true;

    } else {
        std::cout << "\nInvalid answer. Please try again...\n";
        system(PAUSE);
        return show_registration_ui();
    }

    std::cout << "Please enter your information\n";
    
    auto email_address = get_line("Email Address: ");
    auto last_name = get_line("Last Name: ");
    auto first_name = get_line("First Name: ");
    auto middle_name = get_line("Middle Name: ");

    std::cout << "\nPlease enter your login credentials\n";

    auto username = get_line("Username: ");
    auto password = get_line("Password: ");
    auto confirm_password = get_line("Confirm Password: ");

    while (password != confirm_password) {
        std::cout << "Passwords do not match. Please try again.\n";
        system("PAUSE");

        password = get_line("Password: ");
        confirm_password = get_line("Confirm Password: ");
    }

    auto hashed_pw = hash_password(password);
    
    User new_user(
        "0",
        username,
        hashed_pw,
        email_address,
        last_name,
        first_name,
        middle_name,
        is_instructor
    );

    if (create_user(new_user)) {
        std::cout << "\nAccount successfully created. Returning to main menu...\n";
        system(PAUSE);
        return;

    } else {
        std::cout << "\nFailed to create new user. Username or email is already taken...\n";
        system(PAUSE);
        return;
    } 
}

void show_main_menu() {
    system(CLEAR_SCREEN);

    std::cout << "What would you like to do?\n";
    std::cout << "[1] Login\n";
    std::cout << "[2] Register\n";
    std::cout << "[3] Exit\n";

    auto answer = get_line();

    if (answer == "1") {
        show_login_ui();

    } else if (answer == "2") {
        show_registration_ui();

    } else if (answer == "3") {
        return;
    
    } else {
        std::cout << "\nInvalid answer. Please try again\n";
        system(PAUSE);
    }

    return show_main_menu();
}

int main() {
    show_main_menu();

    std::cout << "Thank you for using the program...\n";

    return 0;
}
