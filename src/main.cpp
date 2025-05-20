#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"
#include "Connection.hpp"

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

std::string extract_header(std::vector<uint8_t>& data) {
    // Define the delimiter sequence to find
    const std::string delimiter = "\r\n\r\n";

    // Search for delimiter in data
    auto it = std::search(data.begin(), data.end(),
                          delimiter.begin(), delimiter.end());

    if (it == data.end()) {
        // Delimiter not found
        return "";
    }

    // Calculate position after delimiter
    size_t header_end_pos = std::distance(data.begin(), it) + delimiter.size();

    // Extract header bytes into string
    std::string header(data.begin(), data.begin() + header_end_pos);

    // Remove extracted header bytes from original vector
    data.erase(data.begin(), data.begin() + header_end_pos);

    return header;
}

int main() {
    RequestHandler req_handler("/home/kgang.maleho/Develop/http-server/test/");

    std::string request = "GET /index.html HTTP/1.1\r\n"
    "Host: /index.html\r\n"
    "User-Agent: MyClient/1.0\r\n"
    "Accept: */*\r\n"
    "Connection: close\r\n"
    "\r\n";
    int port = 8080;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    boost::asio::ip::tcp::socket socket(io_context);
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(socket);
    while (true) {
        acceptor.accept(socket);
        std::vector<uint8_t> resp(4080);
        // size_t len = connection->socket_read(resp);
        // std::string delim = "\r\n\r\n";
        // auto it = std::search(resp.begin(), resp.end(), delim.begin(), delim.end());
        // auto end = std::distance(resp.begin(), it) + delim.size();
        // std::string header(resp.begin(), resp.begin() + end);
        // std::string header = extract_header(resp);
        
        std::string header;
        boost::asio::streambuf buf;
        connection->read_header(header, buf);
        std::cout << "Header:" << std::endl;
        std::cout << header << std::endl;
        HttpRequest req;
        req.parse(header);

        std::cout << "Body:" << std::endl;
        auto data = buf.data();
        std::vector<uint8_t> body(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));
        int first_size = body.size();
        int body_len = std::stoul(req.headers.at("Content-Length"));
        body.resize(body_len);
        connection->read_exact(body, body_len - first_size);
        std::string bufstring (body.begin(), body.end());
        std::cout << bufstring << std::endl;

        // body.insert(body.begin(), data.begin(), data.end());
        // std::string bodystring(body.begin(), body.end());
        // std::cout << bodystring << std::endl;
        // connection->socket_read(body);

        // std::vector<uint8_t> body;
        // len = connection->socket_read(body);
        // HttpRequest http_request;
        // http_request.parse(resp);

        // req_handler.process_request(http_request);
        socket.close();
    }

    return 0; 
}  