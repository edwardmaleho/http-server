#pragma once

#include <string>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <boost/asio.hpp>

class RequestHandler {
private:
    std::string document_root;

    HttpResponse handle_get_request(const HttpRequest& request) {
        std::vector<uint8_t> vec;
        std::cout << "This is a get request!" << std::endl;
        return HttpResponse::build(HttpStatus::OK, "text/plain", vec);
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
        else {
            return HttpResponse::build_error(HttpStatus::METHOD_NOT_ALLOWED);
        }
    }

};