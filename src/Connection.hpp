#pragma once

#include <boost/asio.hpp>
#include <iostream>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(boost::asio::ip::tcp::socket& socket) : socket(socket) {
        
    }
    std::vector<uint8_t> extract_remaining();
    size_t read_header(std::string& header);
    size_t read_exact(std::vector<uint8_t>& buffer, int new_size);
    size_t socket_read(std::vector<uint8_t>& buffer);
    void socket_write(const std::vector<uint8_t>& message);
private:
    boost::asio::ip::tcp::socket& socket;
    boost::asio::streambuf buf;
};

