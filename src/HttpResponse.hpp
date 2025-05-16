#pragma once

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include "http_utils.hpp"

std::string SERVER_NAME {"My Server"};

namespace HttpStatus {
    const std::string OK {"200 OK"};
    const std::string CREATED {"201 Created"};
    const std::string NO_CONTENT {"204 No Content"};

    const std::string MOVED_PERMANENTLY {"301 Moved Permanently"};
    const std::string FOUND {"302 Found"};
    const std::string NOT_MODIFIED {"304 Not Modified"};

    const std::string BAD_REQUEST {"400 Bad Request"};
    const std::string UNAUTHORIZED {"401 Unauthorized"};
    const std::string FORBIDDEN {"403 Forbidden"};
    const std::string NOT_FOUND {"404 Not Found"};

    const std::string INTERNAL_SERVER_ERROR {"500 Internal Server Error"};
    const std::string BAD_GATEWAY {"502 Bad Gateway"};
    const std::string SERVICE_UNAVAILABLE {"503 Service Unavailable"};
    const std::string GATEWAY_TIMEOUT {"Gateway Timeout"};
}

struct HttpResponse {
    std::string http_version = "HTTP/1.1";
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::vector<uint8_t> body;

    void set_header(const std::string& name, const std::string& value) {
        headers[name] = value;
    }

    HttpResponse() {
        set_header("Server", SERVER_NAME);
        set_header("Date", get_date()); 
        set_header("Connection", "keep-alive");
    }

    std::string to_string() {
        std::stringstream stream;
        stream << http_version << " " << status_message << "\r\n";
        for (const auto& header : headers) {
            stream << header.first << ": " << header.second << "\r\n";
        }
        stream << "\r\n";
        stream << reinterpret_cast<const char*>(body.data()) << "\r\n";
        return stream.str();
    }

    static HttpResponse build(const std::string& status, const std::string& content_type, const std::vector<uint8_t>& body) {
        HttpResponse http_response;
        http_response.status_message = status;
        http_response.set_header("Content-Type", content_type);
        http_response.body = body;
        http_response.set_header("Content-Length", std::to_string(body.size()));
        return http_response;
    }
};