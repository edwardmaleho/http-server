#include "RequestHandler.hpp"

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

    std::string create_user(const HttpRequest& req) {
        return "Created Successfully";
    }

    std::map<std::string, std::function<std::string(const HttpRequest& req)>> get_routes = {
        {"/api/user", return_user},
        {"/api/about", return_about},
        {"/api/status", return_server_status}
    };
    
    std::map<std::string, std::function<std::string(const HttpRequest& req)>> post_routes = {
        {"/newuser", create_user}
    };
}

HttpResponse RequestHandler::handle_get_request(const HttpRequest& request) {
    std::string path = request.target_url;
    if (path == "/") {
        path = "/index.html";
    }
    std::string full_path = document_root + path;
    if (Routes::get_routes.count(path)) {
        std::string body;
        body = Routes::get_routes[path](request);
        return HttpResponse::build(server_name, HttpStatus::OK, "text/plain", Utils::convert_to_vector(body));
    }
    else if (std::filesystem::exists(full_path)) {
        std::vector<uint8_t> body;
        if (FileIO::read_file_content(full_path, body)) {
            return HttpResponse::build(server_name, HttpStatus::OK, Utils::get_content_type(path), body);
        } else {
            return HttpResponse::build_error(server_name, HttpStatus::NOT_FOUND, path + " not found");
        }
    }
    else {
        return HttpResponse::build_error(server_name, HttpStatus::NOT_FOUND, path + " Not found");
    }
}

HttpResponse RequestHandler::handle_post_request(const HttpRequest& request) {
    std::string full_path = document_root + request.target_url;
    if (!request.headers.count("Content-Length")) {
        return HttpResponse::build_error(server_name, HttpStatus::NO_CONTENT, request.target_url + " contains no content");
    }
    if (!std::filesystem::exists(full_path)) {
        FileIO::write_to_file(full_path, request.body);
        return HttpResponse::build(server_name, HttpStatus::OK, "", {});
    }
    return HttpResponse::build_error(server_name, HttpStatus::NOT_FOUND, request.target_url + " Not found");
}

HttpResponse RequestHandler::process_request(HttpRequest& request) {
    if (request.http_version != "HTTP/1.1") {
        return HttpResponse::build_error(server_name, HttpStatus::HTTP_VERSION_NOT_SUPPORTED);
    }
    if (request.request_type == "GET") {
        return handle_get_request(request);
    }
    if (request.request_type == "POST") {
        return handle_post_request(request);
    }
    if (request.request_type == "PUT") {
        //TODO: Implement PUT
    }
    if (request.request_type == "DELETE") {
        //TODO: Implement DELETE
    }
    return HttpResponse::build_error(server_name, HttpStatus::METHOD_NOT_ALLOWED);
}
