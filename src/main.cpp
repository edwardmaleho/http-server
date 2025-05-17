#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"

int tests() {
    // HttpResponse http_response = HttpResponse::build(HttpStatus::BAD_GATEWAY, "text/plain", "this is some information");
    // std::cout << http_response.to_string(); 
 
    // std::cout << get_date() << std::endl;
    // std::vector<uint8_t> content;
    // read_file_content("./testfile.txt", content);
  
    // std::cout.write(reinterpret_cast<const char*>(content.data()), content.size()) << std::endl;

    // std::string file {"help.html"};
    // std::cout << get_content_type(file) << std::endl; 

    std::vector<uint8_t> data;
    std::string file_name {"favico.ico"};
    if (!read_file_content(file_name, data)) {
        std::cout << "Failed to read file" << file_name << std::endl;
        return 1;
    }
    
    HttpResponse http_response = HttpResponse::build(HttpStatus::OK, get_content_type(file_name), data); 
    std::cout << http_response.to_string() << std::endl;

    std::string request = "GET /index.html HTTP/1.1\r\n"
    "Host: /index.html\r\n"
    "User-Agent: MyClient/1.0\r\n"
    "Accept: */*\r\n"
    "Connection: close\r\n"
    "\r\n";

    HttpRequest http_request;
    http_request.parse(request);

    std::cout << http_request.http_version << http_request.request_type << http_request.target_url << std::endl;
    return 0;
}


int main() {
    RequestHandler req_handler("/home/kgang.maleho/Develop/http-server/test/");

    std::string request = "GET /index.html HTTP/1.1\r\n"
    "Host: /index.html\r\n"
    "User-Agent: MyClient/1.0\r\n"
    "Accept: */*\r\n"
    "Connection: close\r\n"
    "\r\n";

    HttpRequest http_request;
    http_request.parse(request);

    req_handler.process_request(http_request);
    
    return 0; 
}  