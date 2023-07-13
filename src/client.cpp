#include <data_structures/avl_tree.hpp>
#include <user.hpp>
#include <answers.hpp>

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

            user = new User(
                "0",
                username,
                hashed_pw,
                get_value(response, "email_address"),
                get_value(response, "last_name"),
                get_value(response, "first_name"),
                get_value(response, "middle_name")
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
    
    auto response = send_post_request(utility::conversions::to_string_t(SERVER_URL + "/register"), body);

    if (response.has_field(U("success"))) {
        auto is_success = get_value(response, "success");
        return is_success == "true";
    
    } else {
        return false;
    }
}

bool check_exam_code(std::string code) {
    // TK
    
    return true;
}

void upload_answers(const Answers& answers) {
    // TK
}

void input_exam(int number_of_items) {
    system(CLEAR_SCREEN);

    int current_number = 1;

    Answers answers;

    while (current_number <= number_of_items) {
        auto answer = get_line(std::to_string(current_number) + ". ");

        if (answer == "-") {
            answers.undo_answer();
            current_number--;

            system(CLEAR_SCREEN);

            for (int i = 0; i < current_number - 1; i++) {
                std::cout << (i + 1) << ". " << answers[i] << std::endl;
            }

        } else {
            answers.add_answer(answer);
            current_number++;
        }
    }

    upload_answers(answers);

    std::cout << "\nAnswer sheet was submitted successfully.\n";
    system(PAUSE);
}

void start_exam() {
    // TK
    
    system(CLEAR_SCREEN);

    std::cout << "======= EXAM ========\n";
    
    auto exam_code = get_line("Exam code: ");

    if (check_exam_code(exam_code)) {
        // TK
        input_exam(10);

    } else {
        std::cout << "\nExam code does not exist...\n";
        system(PAUSE);
    }
}

void show_exam_history() {
    // TK
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
        return show_student_dashboard();

    } else {
        std::cout << "\nIncorrect username or password. Please try again...\n";
        system(PAUSE);
        return;
    }
}

void show_registration_ui() {
    system(CLEAR_SCREEN);

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
        middle_name
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
