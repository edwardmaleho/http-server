#pragma once

#include <string>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"

class RequestHandler {
private:
    std::string document_root;

    HttpResponse handle_get_request(const HttpRequest& requeset) {

    }

public:
    RequestHandler(const std::string& doc_root) : document_root(doc_root) {

    }

    HttpResponse process_request(HttpRequest& request) {

    }

};