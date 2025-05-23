#pragma once

#include <string>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <boost/asio.hpp>

namespace Routes {
    extern std::map<std::string, std::function<std::string(const HttpRequest& req)>> get_routes;
    extern std::map<std::string, std::function<std::string(const HttpRequest& req)>> post_routes;

    std::string return_user(const HttpRequest& req);
    std::string return_about(const HttpRequest& req);
    std::string return_server_status(const HttpRequest& req);
    std::string create_user(const HttpRequest& req);
}

class RequestHandler {
private:
    std::string document_root;
    std::string server_name;

    HttpResponse handle_get_request(const HttpRequest& request);
    HttpResponse handle_post_request(const HttpRequest& request);

public:
    RequestHandler(const std::string& server_name, const std::string& doc_root) : server_name(server_name), document_root(doc_root) {}
    HttpResponse process_request(HttpRequest& request);

};