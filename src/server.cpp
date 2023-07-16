#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include <user.hpp>
#include <data_structures/avl_tree.hpp>
#include <exam.hpp>
#include <score.hpp>
#include <result.hpp>
#include <sorting/merge_sort.hpp>

#include <fstream>

#define TO_JSON_STRING(s) json::value::string(utility::conversions::to_string_t(s))

using namespace web;
using namespace http;
using namespace http::experimental::listener;



AVLTree<User> users;
AVLTree<Exam> exams;


class RestApiHandler
{
public:
    json::value user_db;
    json::value exam_db;
    int exam_count = 0;
    int user_count = 0;

    RestApiHandler() {
        std::string json_string;
        std::ifstream input_file("user_db.json");

        if (input_file.is_open()) {
            std::getline(input_file, json_string);
            input_file.close();

            user_db = json::value::parse(utility::conversions::to_string_t(json_string));
                        
            for (auto& user_json : user_db.as_array()) {
                auto user = to_user(user_json);
                users.insert(user);
                user_count++;
            }

        } else {
            std::cout << "Failed to open the file." << std::endl;
        }

        // Processing exam db file
        std::string exam_db_string;
        std::ifstream exam_db_file("exam_db.json");

        if (exam_db_file.is_open()) {
            std::getline(exam_db_file, exam_db_string);
            exam_db_file.close();

            exam_db = json::value::parse(utility::conversions::to_string_t(exam_db_string));
                        
            for (auto& exam_json : exam_db.as_array()) {
                auto exam = to_exam(exam_json);
                exams.insert(exam);
                exam_count++;
            }

        } else {
            std::cout << "Failed to open exam db file." << std::endl;
        }
    }

    ~RestApiHandler() {
        json::value new_user_db;
        user_count = 0;

        auto current_user = users.subtree_first(users.get_root());

        while (current_user) {
            User& user = current_user->data;
            //user.print_info();
            auto user_json = to_json_value(user);
            new_user_db[user_count] = user_json;
            user_count++;

            try {
                current_user = users.successor(current_user);
            
            } catch (std::runtime_error& e) {
                break;
            }
        }

        json::value new_exam_db;
        exam_count = 0;

        auto current_exam = exams.subtree_first(exams.get_root());

        while (current_exam) {
            Exam& exam = current_exam->data;
            //std::cout << exam.exam_code << std::endl;
            auto exam_json = to_json_value(exam);
            new_exam_db[exam_count] = exam_json;
            exam_count++;

            try {
                current_exam = exams.successor(current_exam);
            
            } catch (std::runtime_error& e) {
                break;
            }
        }

        std::string serialized = utility::conversions::to_utf8string(new_user_db.serialize());
        std::ofstream output_file("user_db.json");

        if (output_file.is_open()) {
            output_file << serialized;
            output_file.close();
            std::cout << "String saved to the file successfully." << std::endl;
        } else {
            std::cout << "Failed to open the file." << std::endl;
        }

        // Process exam db
        std::string exam_db_string = utility::conversions::to_utf8string(new_exam_db.serialize());
        //std::string exam_db_string = utility::conversions::to_utf8string(exam_db.serialize());
        std::ofstream exam_db_file("exam_db.json");

        if (exam_db_file.is_open()) {
            exam_db_file << exam_db_string;
            exam_db_file.close();
            std::cout << "String saved to the file successfully." << std::endl;
        } else {
            std::cout << "Failed to open the file." << std::endl;
        }
    }

    json::value to_json_value(Exam& exam) {
        json::value json_value;

        json::value answer_key;
        for (int i = 0; i < exam.answer_key.get_size(); i++) {
            answer_key[i] = TO_JSON_STRING(exam.answer_key[i]);
        }

        json_value[U("uploader")] = TO_JSON_STRING(exam.uploader);
        json_value[U("code")] = TO_JSON_STRING(exam.exam_code);
        json_value[U("title")] = TO_JSON_STRING(exam.title);
        json_value[U("answer_key")] = answer_key;

        if (exam.scores.get_root() != NULL) {
            json::value scores;
            int score_count = 0;

            auto current = exam.scores.subtree_first(exam.scores.get_root());

            while (current) {
                Score& score = current->data;
                auto score_json = to_json_value(score);
                scores[score_count] = score_json;
                score_count++;

                try {
                    current = exam.scores.successor(current);
                
                } catch (std::runtime_error& e) {
                    break;
                }
            }

            json_value[U("scores")] = scores;
        }

        return json_value;
    }

    json::value to_json_value(User& user) {
        json::value user_info;
        user_info[U("username")] = TO_JSON_STRING(user.get_username());
        user_info[U("email_address")] = TO_JSON_STRING(user.get_email_address());
        user_info[U("pw_hash")] = TO_JSON_STRING(user.get_password_hash());
        user_info[U("last_name")] = TO_JSON_STRING(user.get_last_name());
        user_info[U("first_name")] = TO_JSON_STRING(user.get_first_name());
        user_info[U("middle_name")] = TO_JSON_STRING(user.get_middle_name());

        if (user.is_instructor) {
            user_info[U("is_instructor")] = TO_JSON_STRING("true");
        }

        if (!user.submitted_scores.empty()) {
            json::value scores;

            std::cout << "Score count: " << user.score_count << std::endl;

            for (int i = 0; i < user.score_count; i++) {
                scores[i] = to_json_value(user.submitted_scores[i]);
            }

            std::cout << "Scores json: " << utility::conversions::to_utf8string(scores.serialize()) << std::endl;

            user_info[U("scores")] = scores;
        }

        if (!user.submitted_exams.empty()) {
            json::value exams;

            for (int i = 0; i < user.exam_count; i++) {
                exams[i] = to_json_value(user.submitted_exams[i]);
            }

            user_info[U("exams")] = exams;
        }

        return user_info;
    }

    json::value to_json_value(Score& score) {
        json::value json_value;

        json::value answers;
        for (int i = 0; i < score.answers.get_size(); i++) {
            answers[i] = TO_JSON_STRING(score.answers[i]);
        }

        json_value[U("uploader")] = TO_JSON_STRING(score.uploader);
        json_value[U("code")] = TO_JSON_STRING(score.exam_code);
        json_value[U("answers")] = answers;
        
        return json_value;
    }

    Exam to_exam(json::value& json_value) {
        auto uploader = get_value(json_value, "uploader");
        auto code = get_value(json_value, "code");
        auto title = get_value(json_value, "title");
        
        Exam exam(uploader, title, code);

        auto answer_key = json_value.at(U("answer_key")).as_array();
        for (auto& answer : answer_key) {
            auto answer_w = answer.as_string();
            std::string answer_string(answer_w.begin(), answer_w.end());
            exam.answer_key.push_back(answer_string);
        }

        if (json_value.has_field(U("scores"))) {
            auto scores = json_value.at(U("scores")).as_array();

            for (auto& score : scores) {
                exam.scores.insert(to_score(score));
                exam.score_count++;
            }
        }

        return exam;
    }

    User to_user(json::value& user_json) {
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

        if (user_json.has_field(U("scores"))) {
            auto scores = user_json.at(U("scores")).as_array();

            for (auto& score_json : scores) {
                auto score = to_score(score_json);
                user.submitted_scores.push_back(to_score(score_json));
                user.score_count++;
            }
        }

        if (user_json.has_field(U("exams"))) {
            auto exams = user_json.at(U("exams")).as_array();

            for (auto& exam : exams) {
                user.submitted_exams.push_back(to_exam(exam));
                user.exam_count++;
            }
        }

        return user;
    }

    Score to_score(json::value& json_value) {
        auto uploader = get_value(json_value, "uploader");
        auto code = get_value(json_value, "code");
        auto answers = json_value.at(U("answers")).as_array();

        Score score(uploader, code);

        for (auto& answer : answers) {
            auto answer_w = answer.as_string();
            auto answer_string = std::string(answer_w.begin(), answer_w.end());
            score.answers.push_back(answer_string);
        }

        return score;
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

    bool create_exam(Exam& exam) {
        try {
            exams.insert(exam);
            exam_count++;

            auto& user = users.search(User(exam.uploader))->data;
            user.submitted_exams.push_back(exam);
            user.exam_count++;

            std::cout << "Created exam: \n";
            std::cout << "Uploader: " << exam.uploader << std::endl;
            std::cout << "Title: " << exam.title << std::endl;
            std::cout << "Code: " << exam.exam_code << std::endl;

            return true;
        
        } catch (std::runtime_error& e) {
            return false;
        }
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

                } else if (path == "/create_exam") {
                    std::cout << "Creating exam: \n";

                    auto uploader = get_value(post_data, "uploader");
                    auto title = get_value(post_data, "title");
                    auto code = get_value(post_data, "code");

                    std::cout << "Uploader: " << uploader << std::endl;
                    std::cout << "Title: " << title << std::endl;
                    std::cout << "Exam code: " << code << std::endl;
                    std::cout << "Answer Key: " << std::endl;

                    json::value answer_key = post_data.at(U("answer_key"));

                    Exam exam(uploader, title, code);

                    int counter = 1;
                    for (const auto& item : answer_key.as_array()) {
                        auto answer_w = item.as_string();
                        std::string answer(answer_w.begin(), answer_w.end());

                        exam.answer_key.push_back(answer);

                        std::cout << counter << ". " << answer << std::endl;
                        counter++;
                    }

                    bool success = create_exam(exam);

                    json::value response;
                    response[U("success")] = json::value::string(success ? U("true") : U("false"));
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/check_exam") {
                    std::cout << "Checking exam code: " << std::endl;

                    auto code = get_value(post_data, "code");

                    auto exam_node = exams.search(Exam(code));

                    bool success = exam_node != NULL;

                    json::value response;
                    response[U("success")] = json::value::string(success ? U("true") : U("false"));

                    if (success) {
                        auto exam = exam_node->data;
                        response[U("number_of_items")] = exam.answer_key.get_size();
                        response[U("title")] = TO_JSON_STRING(exam.title);
                    }

                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/upload_score") {
                    std::cout << "Uploading score: " << std::endl;

                    auto uploader = get_value(post_data, "uploader");
                    auto code = get_value(post_data, "code");

                    Score score(uploader, code);

                    std::cout << "Uploader: " << uploader << std::endl;
                    std::cout << "Code: " << code << std::endl;

                    auto answers = post_data.at(U("answers")).as_array();

                    std::cout << "Answers: " << std::endl;

                    int counter = 1;
                    for (const auto& item : answers) {
                        auto item_w = item.as_string();
                        std::string item(item_w.begin(), item_w.end());

                        score.answers.push_back(item);

                        std::cout << counter << ". " << item << std::endl;
                        
                        counter++;
                    }


                    bool success = true;

                    try {
                        auto& exam = exams.search(Exam(code))->data;
                        exam.scores.insert(score);
                        exam.score_count++;

                        auto& user = users.search(User(uploader))->data;
                        user.submitted_scores.push_back(score);
                        user.score_count++;

                    } catch (std::runtime_error& e) {
                        success = false;
                    }
                    
                    json::value response;
                    response[U("success")] = json::value::string(success ? U("true") : U("false"));
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/get_exams") {
                    std::cout << "Getting exams for: ";

                    auto username = get_value(post_data, "username");
                    
                    std::cout << username << std::endl;

                    auto user = users.search(User(username))->data;

                    json::value response;

                    if (user.score_count > 0) {
                        response[U("count")] = user.score_count;

                        json::value scores;

                        for (int i = 0; i < user.score_count; i++) {
                            Score& score = user.submitted_scores[i];
                            Exam& exam = exams.search(Exam(score.exam_code))->data;

                            json::value result;
                            result[U("code")] = TO_JSON_STRING(exam.exam_code);
                            result[U("title")] = TO_JSON_STRING(exam.title);
                            result[U("item_count")] = exam.answer_key.get_size();

                            int grade = 0;
                            for (int i = 0; i < exam.answer_key.get_size(); i++) {
                                if (score.answers[i] == exam.answer_key[i]) {
                                    grade++;
                                }
                            }

                            result[U("grade")] = grade;

                            scores[i] = result;
                        }

                        response[U("scores")] = scores;

                    } else {
                        response[U("count")] = 0;
                    }

                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/search_exam_with_code") {
                    std::cout << "Searching exam result with code: \n";
                    json::value response;
                    
                    auto username = get_value(post_data, "username");
                    auto code = get_value(post_data, "code");

                    std::cout << "Client: " << username << std::endl;
                    std::cout << "Code: " << code << std::endl;

                    auto exam_query = exams.search(Exam(code));

                    if (!exam_query) {
                        response[U("none")] = false;
                        request.reply(status_codes::OK, response);
                        return;
                    }

                    Exam& exam = exam_query->data;

                    auto score_query = exam.scores.search(Score(username, code));

                    if (!score_query) {
                        response[U("untaken")] = false;
                        response[U("title")] = TO_JSON_STRING(exam.title);
                        response[U("item_count")] = exam.answer_key.get_size();
                        response[U("score_count")] = exam.score_count;
                        request.reply(status_codes::OK, response);
                        return;
                    }

                    Score& score = score_query->data;

                    json::value answers;
                    json::value answer_key;

                    for (int i = 0; i < exam.answer_key.get_size(); i++) {
                        answers[i] = TO_JSON_STRING(score.answers[i]);
                        answer_key[i] = TO_JSON_STRING(exam.answer_key[i]);
                    }

                    response[U("answers")] = answers;
                    response[U("answer_key")] = answer_key;
                    
                    response[U("title")] = TO_JSON_STRING(exam.title);
                    response[U("item_count")] = exam.answer_key.get_size();
                    response[U("score_count")] = exam.score_count;

                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/my_exams") {
                    std::cout << "Fetching all exams created by: ";
                    json::value response;

                    auto username = get_value(post_data, "username");

                    std::cout << username << std::endl;

                    auto& user = users.search(User(username))->data;

                    if (user.exam_count == 0) {
                        response[U("none")] = true;
                        request.reply(status_codes::OK, response);
                        return;
                    }

                    json::value exams;

                    for (int i = 0; i < user.exam_count; i++) {
                        Exam& exam = user.submitted_exams[i];

                        json::value exam_info;

                        exam_info[U("title")] = TO_JSON_STRING(exam.title);
                        exam_info[U("code")] = TO_JSON_STRING(exam.exam_code);

                        exams[i] = exam_info;
                    }

                    response[U("exam_count")] = user.exam_count;
                    response[U("exams")] = exams;
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/view_exam") {
                    std::cout << "View item analysis of exam: ";

                    auto username = get_value(post_data, "username");
                    auto code = get_value(post_data, "code");

                    std::cout << code << std::endl;

                    User& user = users.search(User(username))->data;
                    Exam& exam = exams.search(Exam(code))->data;

                    auto exam_json = to_json_value(exam);

                    json::value response;
                    response[U("exam")] = exam_json;
                    request.reply(status_codes::OK, response);
                    return;

                } else if (path == "/view_rankings") {
                    std::cout << "Viewing rankings for: ";

                    auto code = get_value(post_data, "code");
                    auto username = get_value(post_data, "username");

                    std::cout << code << std::endl;

                    User& user = users.search(User(username))->data;
                    Exam& exam = exams.search(Exam(code))->data;

                    json::value response;

                    if (exam.score_count == 0) {
                        response[U("none")] = true;
                        request.reply(status_codes::OK, response);
                        return;  
                    }

                    Array<Result> results;

                    AVLTree<Score>::Node* current = NULL;
                    
                    for (int i = 0; i < exam.score_count; i++) {
                        if (!current) {
                            current = exam.scores.subtree_first(exam.scores.get_root());
                        } else {
                            current = exam.scores.successor(current);
                        }

                        Score& score = current->data;
                        User& user = users.search(User(score.uploader))->data;
                        
                        Result result(&user, &exam, &score);
                        results.push_back(result);
                    }

                    merge_sort(results);

                    json::value rankings;
                    int counter = 0;

                    for (int i = results.get_size() - 1; i >= 0; i--) {
                        Result& result = results[i];

                        json::value result_json;
                        result_json[U("last_name")] = TO_JSON_STRING(result.user->get_last_name());
                        result_json[U("first_name")] = TO_JSON_STRING(result.user->get_first_name());
                        result_json[U("middle_name")] = TO_JSON_STRING(result.user->get_middle_name());
                        result_json[U("grade")] = result.grade;

                        rankings[counter] = result_json;
                        counter++;
                    }

                    response[U("title")] = TO_JSON_STRING(exam.title);
                    response[U("score_count")] = exam.score_count;
                    response[U("rankings")] = rankings;
                    response[U("perfect")] = exam.answer_key.get_size();
                    request.reply(status_codes::OK, response);
                    return;                   

                } else {

                }

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
