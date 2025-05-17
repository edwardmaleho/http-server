#pragma once

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

size_t socket_read(tcp::socket& socket, std::vector<uint8_t>& buffer, boost::system::error_code& error) {
    error.clear();
    try {
        size_t len = socket.read_some(boost::asio::buffer(buffer), error);
        return len;
    } 

    catch (boost::system::error_code& e) {
        std::cout << e.what() << std::endl;
    }
}

void socket_write(tcp::socket& socket, std::string message, boost::system::error_code& error) {
    error.clear();
    try {
        boost::asio::write(socket, boost::asio::buffer(message), error);
    } 
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
