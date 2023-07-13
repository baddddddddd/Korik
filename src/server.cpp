#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include <user.hpp>
#include <data_structures/avl_tree.hpp>

#include <fstream>

#define TO_JSON_STRING(s) json::value::string(utility::conversions::to_string_t(s))

using namespace web;
using namespace http;
using namespace http::experimental::listener;



AVLTree<User> users;


class RestApiHandler
{
public:
    json::value user_db;
    int user_count = 0;

    RestApiHandler() {
        std::string json_string;
        std::ifstream input_file("user_db.json");

        if (input_file.is_open()) {
            std::getline(input_file, json_string);
            input_file.close();

            user_db = json::value::parse(utility::conversions::to_string_t(json_string));
                        
            for (const auto& user_json : user_db.as_array()) {
                bool is_instructor = false;

                if (user_json.has_field(U("is_instructor"))) {
                    is_instructor = true;
                }

                User user(
                    "0",
                    get_value(user_json, "username"),
                    get_value(user_json, "pw_hash"),
                    get_value(user_json, "email_address"),
                    get_value(user_json, "last_name"),
                    get_value(user_json, "first_name"),
                    get_value(user_json, "middle_name"),
                    is_instructor
                );

                users.insert(user);

                user_count++;
            }

        } else {
            std::cout << "Failed to open the file." << std::endl;
        }
    }

    ~RestApiHandler() {
        std::string serialized = utility::conversions::to_utf8string(user_db.serialize());
        std::ofstream output_file("user_db.json");

        if (output_file.is_open()) {
            output_file << serialized;
            output_file.close();
            std::cout << "String saved to the file successfully." << std::endl;
        } else {
            std::cout << "Failed to open the file." << std::endl;
        }
    }

    bool create_user(
        std::string username,
        std::string password_hash,
        std::string email_address,
        std::string last_name,
        std::string first_name,
        std::string middle_name,
        bool is_instructor = false
    ) {
        User user(
            "0",
            username,
            password_hash,
            email_address,
            last_name,
            first_name,
            middle_name,
            is_instructor
        );

        try {
            users.insert(user);

            json::value user_info;
            user_info[U("username")] = TO_JSON_STRING(user.get_username());
            user_info[U("email_address")] = TO_JSON_STRING(user.get_email_address());
            user_info[U("pw_hash")] = TO_JSON_STRING(user.get_password_hash());
            user_info[U("last_name")] = TO_JSON_STRING(user.get_last_name());
            user_info[U("first_name")] = TO_JSON_STRING(user.get_first_name());
            user_info[U("middle_name")] = TO_JSON_STRING(user.get_middle_name());

            if (is_instructor) {
                user_info[U("is_instructor")] = TO_JSON_STRING("true");
            }

            user_db[user_count] = user_info;
            user_count++;
            
            std::cout << "Registered: ";
            user.print_info();
            
            return true;

        } catch (std::runtime_error& e) {
            return false;
        }        
    }

    bool verify_login(std::string username, std::string pw_hash) {
        User user(
            "0",
            username,
            pw_hash,
            "0",
            "0",
            "0",
            "0"
        );

        auto result = users.search(user);

        if (result != NULL) {
            auto queried_user = result->data;
            return queried_user.get_password_hash() == pw_hash;

        } else {
            return false;
        }
    }

    std::string get_value(const json::value& dict, const std::string& key) {
        const std::wstring& value_w = dict.at(utility::conversions::to_string_t(key)).as_string();
        std::string value(value_w.begin(), value_w.end());

        return value;
    }

    void handle_get(http_request request) {
        json::value response;
        std::wstring pathW = request.request_uri().path();
        std::string path(pathW.begin(), pathW.end());

        if (path == "/" || path == "/home") {
            response[U("message")] = json::value::string(U("Hello, world!"));
            request.reply(status_codes::OK, response);
        
        } else if (path == "/login") {
            
        
        } else if (path == "/register") {

        } else if (path == "/exam") {


        } else {
            response[U("message")] = json::value::string(U("Endpoint not found."));
        }

    }

    void handle_post(http_request request)
    {
        json::value response;
        std::wstring pathW = request.request_uri().path();
        std::string path(pathW.begin(), pathW.end());

        std::cout << "Endpoint for post: " << path << std::endl;

        request.extract_json().then([=](pplx::task<json::value> task) {
            try {
                const json::value& post_data = task.get();

                if (path == "/login") {
                    std::cout << "Logging in: \n";

                    auto username = get_value(post_data, "username");
                    auto pw_hash = get_value(post_data, "pw_hash");

                    bool success = verify_login(username, pw_hash);

                    json::value response;

                    if (success) {
                        auto user = users.search(User(
                            "0",
                            username,
                            pw_hash,
                            "0",
                            "0",
                            "0",
                            "0"
                        ))->data;

                        response[U("success")] = TO_JSON_STRING("true");
                        response[U("username")] = TO_JSON_STRING(user.get_username());
                        response[U("email_address")] = TO_JSON_STRING(user.get_email_address());
                        response[U("last_name")] = TO_JSON_STRING(user.get_last_name());
                        response[U("first_name")] = TO_JSON_STRING(user.get_first_name());
                        response[U("middle_name")] = TO_JSON_STRING(user.get_middle_name());

                        if (user.is_instructor) {
                            response[U("is_instructor")] = TO_JSON_STRING("true");
                        }

                    } else {
                        response[U("success")] = TO_JSON_STRING("false");
                    }
                
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/register") {
                    std::cout << "registering: \n";

                    auto username = get_value(post_data, "username");
                    auto email_address = get_value(post_data, "email_address");
                    auto pw_hash = get_value(post_data, "pw_hash");
                    auto last_name = get_value(post_data, "last_name");
                    auto first_name = get_value(post_data, "first_name");
                    auto middle_name = get_value(post_data, "middle_name");

                    bool is_instructor = false;

                    if (post_data.has_field(U("is_instructor"))) {
                        is_instructor = true;
                    }

                    bool success = create_user(
                        username,
                        pw_hash,
                        email_address,
                        last_name,
                        first_name,
                        middle_name,
                        is_instructor
                    );
                    
                    json::value response;
                    response[U("success")] = json::value::string(success ? U("true") : U("false"));
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "exam") {

                } else {

                }


                // Process the POST data
                // ...

                // Prepare the response
                // json::value response;
                // response[U("message")] = json::value::string(U("POST request received and processed"));
// 
                // // Send the response
                // request.reply(status_codes::OK, response);
            }
            catch (const std::exception& ex) {
                // Handle errors
                request.reply(status_codes::BadRequest, ex.what());
            }
        });
    }
};

int main()
{
    std::cout << "Program started.\n";

    http_listener listener(U("http://localhost:8080"));
    RestApiHandler handler;

    listener.support(methods::GET, std::bind(&RestApiHandler::handle_get, &handler, std::placeholders::_1));
    listener.support(methods::POST, std::bind(&RestApiHandler::handle_post, &handler, std::placeholders::_1));

    listener.open().then([]() { std::cout << "Listening for requests..." << std::endl; });

    std::cout << "Press ENTER to exit." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    listener.close().wait();

    return 0;
}
