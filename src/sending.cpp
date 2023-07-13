#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

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


int main()
{
    // Send a GET request
    uri_builder builder(U("http://localhost:8080/home")); // Replace with the appropriate endpoint URL
    http_client client(builder.to_string());

    client.request(methods::GET)
        .then([](http_response response) {
            return response.extract_json();
        })
        .then([](json::value jsonValue) {
            std::string serialized = utility::conversions::to_utf8string(jsonValue.serialize());
            std::cout << "GET Response: " << serialized << std::endl;
        })
        .wait();

    json::value postData;
    postData[U("message")] = json::value::string(U("Hello from POST!"));
    postData[U("id")] = json::value::string(U("12345"));
    auto res = send_post_request(U("http://localhost:8080/custom"), postData);

    std::string serialized = utility::conversions::to_utf8string(res.serialize());
    std::cout << "POST Responawdse: " << serialized << std::endl;

    return 0;
    // Send a POST request
    // uri_builder postBuilder(U("http://localhost:8080/custom")); // Replace with the appropriate endpoint URL
    // http_client postClient(postBuilder.to_string());
// 
    // json::value postData;
    // postData[U("message")] = json::value::string(U("Hello from POST!"));
    // postData[U("id")] = json::value::string(U("12345"));
// 
    // http_request request(methods::POST);
    // request.headers().set_content_type(U("application/json"));
    // request.set_body(postData);
// 
    // postClient.request(request)
    //     .then([](http_response response) {
    //         return response.extract_json();
    //     })
    //     .then([](json::value jsonValue) {
    //         std::string serialized = utility::conversions::to_utf8string(jsonValue.serialize());
    //         std::cout << "POST Response: " << serialized << std::endl;
    //     })
    //     .wait();

    return 0;
}
