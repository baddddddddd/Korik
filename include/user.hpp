#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>

class User {
private:
    std::string id;

    std::string username;
    std::string password_hash;

    std::string email_address;

    std::string last_name;
    std::string first_name;
    std::string middle_name;

public:
    User(
        std::string id,
        std::string username,
        std::string password_hash,
        std::string email_address,
        std::string last_name,
        std::string first_name,
        std::string middle_name
    ) : 
        id(id), 
        username(username), 
        password_hash(password_hash),
        email_address(email_address), 
        last_name(last_name), 
        first_name(first_name), 
        middle_name(middle_name)
    {
    }

    std::string get_username() {
        return username;
    }

    std::string get_password_hash() {
        return password_hash;
    }

    std::string get_email_address() {
        return email_address;
    }

    std::string get_last_name() {
        return last_name;
    }

    std::string get_first_name() {
        return first_name;
    }

    std::string get_middle_name() {
        return middle_name;
    }

    void print_info() {
        std::cout << "User Information: \n";
        std::cout << "ID: " << id << "\n";
        std::cout << "Username: " << username << "\n";
        std::cout << "Email Address: " << email_address << "\n";
        std::cout << "Last Name: " << last_name << "\n";
        std::cout << "First Name: " << first_name << "\n";
        std::cout << "Middle Name: " << middle_name << "\n";
    }
};

#endif // USER_HPP