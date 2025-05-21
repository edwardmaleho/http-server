#pragma once

#include <string>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <boost/asio.hpp>

namespace Routes {
    std::string return_user(const HttpRequest& req) {
        return req.headers.at("User-Agent") + '\n';
    }

    std::string return_about(const HttpRequest& req) {
        return "About page\n";
    }

    std::string return_server_status(const HttpRequest& req) {
        return "The server is fine.\n";
    }

    std::map<std::string, std::function<std::string(const HttpRequest& req)>> get_routes = {
        {"/api/user", return_user},
        {"/api/about", return_about},
        {"/api/status", return_server_status}
    };
}

class RequestHandler {
private:
    std::string document_root;

    HttpResponse handle_get_request(const HttpRequest& request) {
        std::string path = request.target_url;
        std::string full_path = document_root + path;
        if (Routes::get_routes.count(path)) {
            std::string body;
            body = Routes::get_routes[path](request);
            return HttpResponse::build(HttpStatus::OK, "text/plain", convert_to_vector(body));
        }
        else if (std::filesystem::exists(full_path)) {
            std::vector<uint8_t> body;
            if (read_file_content(full_path, body)) {
                return HttpResponse::build(HttpStatus::OK, get_content_type(path), body);
            }
        }
        else {
            return HttpResponse::build_error(HttpStatus::NOT_FOUND, path + "Not found");
        }
    }

public:
    RequestHandler(const std::string& doc_root) : document_root(doc_root) { }

    HttpResponse process_request(HttpRequest& request) {
        if (request.http_version != "HTTP/1.1") {
            return HttpResponse::build_error(HttpStatus::HTTP_VERSION_NOT_SUPPORTED);
        }
        if (request.request_type == "GET") {
            return handle_get_request(request);
        }
        if (request.request_type == "POST") {
            // TODO: Implement POST
        }
        if (request.request_type == "PUT") {
            //TODO: Implement PUT
        }
        if (request.request_type == "DELETE") {
            //TODO: Implement DELETE
        }
        return HttpResponse::build_error(HttpStatus::METHOD_NOT_ALLOWED);
    }

};