#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include <data_structures/avl_tree.hpp>

using namespace web;
using namespace http;
using namespace http::experimental::listener;



class RestApiHandler
{
public:
    RestApiHandler() {}

    void handle_get(http_request request)
    {
        // Prepare the response
        json::value response;
        response[U("message")] = json::value::string(U("Hello, world!"));

        // Send the response
        request.reply(status_codes::OK, response);
    }
};

int main()
{
    std::cout << "Program started.\n";
    
    // Create the HTTP listener
    http_listener listener(U("http://localhost:8080"));

    // Define the request handler
    RestApiHandler handler;

    // Handle GET requests
    listener.support(methods::GET, std::bind(&RestApiHandler::handle_get, &handler, std::placeholders::_1));

    // Start listening for requests
    listener.open().then([]() { std::cout << "Listening for requests..." << std::endl; });

    // Wait for a shutdown signal
    std::cout << "Press ENTER to exit." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    // Close the listener
    listener.close().wait();

    return 0;
}
