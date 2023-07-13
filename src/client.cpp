#include <data_structures/avl_tree.hpp>
#include <user.hpp>
#include <answers.hpp>
#include <exam.hpp>

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
        // TKK DISPLAY SCORE
        system(PAUSE);

    } else {
        std::cout << "\nFailed to upload exam.\n";
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

    // TK RECEIVE GRADE

    delete answers;
}

void show_exam_history() {
    system(CLEAR_SCREEN);


    // TK
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

void show_view_exams_ui() {
    system(CLEAR_SCREEN);
    // TK
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

void show_student_dashboard() {
    system(CLEAR_SCREEN);

    std::cout << "======= STUDENT DASHBOARD =======\n";
    std::cout << "Logged in as: " << user->get_last_name() << ", " << user->get_first_name() << " " << user->get_middle_name() << std::endl;
    
    std::cout << std::endl;
    std::cout << "[1] Enter exam code\n";
    std::cout << "[2] View exam history\n";
    std::cout << "[3] Logout\n";

    auto answer = get_line("\nAnswer: ");

    if (answer == "1") {
        start_exam();

    } else if (answer == "2") {
        show_exam_history();

    } else if (answer == "3") {
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
