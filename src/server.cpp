#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include <user.hpp>

using namespace web;
using namespace http;
using namespace http::experimental::listener;



class RestApiHandler
{
public:
    RestApiHandler() {}

    void create_user(
        std::string username,
        std::string password_hash,
        std::string email_address,
        std::string last_name,
        std::string first_name,
        std::string middle_name
    ) {
        User user(
            "0",
            username,
            password_hash,
            email_address,
            last_name,
            first_name,
            middle_name
        );

        std::cout << "Registered: ";
        user.print_info();
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

                } else if (path == "/register") {
                    std::cout << "registering: \n";

                    auto username = get_value(post_data, "username");
                    auto email_address = get_value(post_data, "email_address");
                    auto pw_hash = get_value(post_data, "pw_hash");
                    auto last_name = get_value(post_data, "last_name");
                    auto first_name = get_value(post_data, "first_name");
                    auto middle_name = get_value(post_data, "middle_name");

                    create_user(
                        username,
                        pw_hash,
                        email_address,
                        last_name,
                        first_name,
                        middle_name
                    );
                    
                    json::value response;
                    response[U("success")] = json::value::string(U("true"));
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "exam") {

                } else {

                }

                // Access individual fields of the POST data
                if (post_data.has_field(U("message")))
                {
                    const std::wstring& messageW = post_data.at(U("message")).as_string();
                    std::string message(messageW.begin(), messageW.end());
                    std::cout << "Received message: " << message << std::endl;
                }

                if (post_data.has_field(U("id")))
                {
                    const std::wstring& idW = post_data.at(U("id")).as_string();
                    std::string id(idW.begin(), idW.end());
                    std::cout << "Received ID: " << id << std::endl;
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
