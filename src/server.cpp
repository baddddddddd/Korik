#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace http;
using namespace http::experimental::listener;

class RestApiHandler
{
public:
    RestApiHandler() {}

    void handle_get(http_request request)
    {
        json::value response;
        std::wstring pathW = request.request_uri().path();
        std::string path(pathW.begin(), pathW.end());

        if (path == "/" || path == "/home")
        {
            response[U("message")] = json::value::string(U("Hello, world! (Root)"));
        }
        else if (path == "/custom")
        {
            response[U("message")] = json::value::string(U("Hello, world! (Custom)"));
        }
        else
        {
            response[U("message")] = json::value::string(U("Endpoint not found."));
        }

        request.reply(status_codes::OK, response);
    }

    void handle_post(http_request request)
    {
        json::value response;
        std::wstring pathW = request.request_uri().path();
        std::string path(pathW.begin(), pathW.end());

        std::cout << "Endpoint for post: " << path << std::endl;

        request.extract_json()
            .then([=](pplx::task<json::value> task) {
                try
                {
                    const json::value& postData = task.get();

                    // Access individual fields of the POST data
                    if (postData.has_field(U("message")))
                    {
                        const std::wstring& messageW = postData.at(U("message")).as_string();
                        std::string message(messageW.begin(), messageW.end());
                        std::cout << "Received message: " << message << std::endl;
                    }

                    if (postData.has_field(U("id")))
                    {
                        const std::wstring& idW = postData.at(U("id")).as_string();
                        std::string id(idW.begin(), idW.end());
                        std::cout << "Received ID: " << id << std::endl;
                    }

                    // Process the POST data
                    // ...

                    // Prepare the response
                    json::value response;
                    response[U("message")] = json::value::string(U("POST request received and processed"));

                    // Send the response
                    request.reply(status_codes::OK, response);
                }
                catch (const std::exception& ex)
                {
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
