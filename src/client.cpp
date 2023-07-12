#include <data_structures/avl_tree.hpp>
#include <user.hpp>
#include <answers.hpp>

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#define CLEAR_SCREEN "cls"
#define PAUSE "pause"

User* user;

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
    std::cout << "Username: " << username << std::endl;
    std::cout << "Password: " << password << std::endl;
    return true;

    // TK
}

bool create_user(User new_user) {
    std::string a = new_user.get_username();

    std::cout << "Username: " << new_user.get_username() << std::endl;
    std::cout << "Email Address: " << new_user.get_email_address() << std::endl;

    return true;
    // TK
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
        // TK

        user = new User(
            "22-08367",
            "vladi02",
            "awdawda",
            "22-08367@g.batstate-u.edu.ph",
            "Jocson",
            "Vladimir",
            "Magsino"
        );

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
