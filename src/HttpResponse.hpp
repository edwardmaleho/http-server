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
    const std::string METHOD_NOT_ALLOWED {"405 Method Not Allowed"};

    const std::string INTERNAL_SERVER_ERROR {"500 Internal Server Error"};
    const std::string BAD_GATEWAY {"502 Bad Gateway"};
    const std::string SERVICE_UNAVAILABLE {"503 Service Unavailable"};
    const std::string GATEWAY_TIMEOUT {"504 Gateway Timeout"};
    const std::string HTTP_VERSION_NOT_SUPPORTED {"505 HTTP Version Not Supported"};
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

    std::vector<uint8_t> to_bytes() {
        std::stringstream stream;
        stream << http_version << " " << status_message << "\r\n";
        for (const auto& header : headers) {
            stream << header.first << ": " << header.second << "\r\n";
        }
        stream << "\r\n";
        std::vector<uint8_t> data = convert_to_vector(stream.str());
        data.insert(data.end(), body.begin(), body.end());
        data.push_back('\r');
        data.push_back('\n');
        // stream << reinterpret_cast<const char*>(body.data()) << "\r\n";
        return data;
    }

    static HttpResponse build(const std::string& status, const std::string& content_type, const std::vector<uint8_t>& body) {
        HttpResponse http_response;
        http_response.status_message = status;
        http_response.set_header("Content-Type", content_type);
        http_response.body = body;
        http_response.set_header("Content-Length", std::to_string(body.size()));
        return http_response;
    }

    static HttpResponse build_error(const std::string& status, const std::string& message="") {
        std::string html =  "<!DOCTYPE html>\n"
                            "<html>\n"
                            "<head><title>Error</title></head>\n"
                            "<body style=\"background:#fee; color:#900; text-align:center; font-family:sans-serif; padding:50px;\">\n"
                            "<h1>" + status + "</h1>\n"
                            "<p>" + message + "</p>\n"
                            "</body>\n"
                            "</html>\n";
        std::vector<uint8_t> buffer(html.begin(), html.end());
        HttpResponse response = HttpResponse::build(status, "text/html", buffer);
        response.headers["Content-Length"] = std::to_string(buffer.size());
        return response;
    }
};