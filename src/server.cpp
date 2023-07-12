#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

int main()
{
    // Create an HTTP client
    http_client client(U("https://api.example.com"));

    // Send a GET request
    uri_builder builder(U("/endpoint"));
    client.request(methods::GET, builder.to_string())
        .then([](http_response response)
    {
        // Process the response
        if (response.status_code() == status_codes::OK)
        {
            // Read the response body as a string
            return response.extract_string();
        }
        else
        {
            throw std::runtime_error("GET request failed");
        }
    })
        .then([](pplx::task<std::string> previousTask)
    {
        // Get the response body string
        std::string responseBody = previousTask.get();
        // Process the response body as needed
        std::cout << "Response: " << responseBody << std::endl;
    })
        .wait();

    // Send a POST request
    uri_builder postBuilder(U("/endpoint"));
    json::value postData;
    postData[U("key")] = json::value::string(U("value"));

    client.request(methods::POST, postBuilder.to_string(), postData.serialize(), U("application/json"))
        .then([](http_response response)
    {
        // Process the response
        if (response.status_code() == status_codes::OK)
        {
            // Read the response body as a string
            return response.extract_string();
        }
        else
        {
            throw std::runtime_error("POST request failed");
        }
    })
        .then([](pplx::task<std::string> previousTask)
    {
        // Get the response body string
        std::string responseBody = previousTask.get();
        // Process the response body as needed
        std::cout << "Response: " << responseBody << std::endl;
    })
        .wait();

    return 0;
}
