#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"
#include "Connection.hpp"

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
    int port = 8080;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    boost::asio::ip::tcp::socket socket(io_context);
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(socket);
    while (true) {
        acceptor.accept(socket);
        RequestHandler handler("MyServer/1.0", "www");

        std::string header;

        connection->read_header(header);
        std::cout << "Header:" << std::endl;
        std::cout << header << std::endl;
        HttpRequest req;
        req.parse(header);

        if (req.headers.count("Content-Length")) {
            std::vector<uint8_t> body;
            int body_len = std::stoul(req.headers.at("Content-Length"));
            connection->read_exact(body, body_len);
            req.body = body;
        }
 
        HttpResponse response = handler.process_request(req);

        connection->socket_write(response.to_bytes());

        socket.close();
    }

    return 0; 
}  