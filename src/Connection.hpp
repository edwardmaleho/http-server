#pragma once

#include <boost/asio.hpp>
#include <iostream>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(std::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket(socket) {
        
    }
    std::vector<uint8_t> extract_remaining();

    void async_read_until(std::function<void(const boost::system::error_code&, std::string)> handler);
    void async_read_exactly(int new_size, std::function<void(const boost::system::error_code&, std::vector<uint8_t>)> handler);

    void async_write(const std::vector<uint8_t>& data, std::function<void(const boost::system::error_code&, size_t bytes_transferred)> handler);

    bool is_open();

    void close();

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::asio::streambuf buf;
};

