#pragma once

#include <boost/asio.hpp>
#include <iostream>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(boost::asio::ip::tcp::socket& socket) : socket(socket) {
        
    }
    std::vector<uint8_t> extract_remaining() {
        auto data = buf.data();
        return std::vector<uint8_t>(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));
    }
    size_t read_header(std::string& header) {
        try {
            size_t header_len = boost::asio::read_until(socket, buf, "\r\n\r\n");
            std::istream header_stream(&buf);
            std::string header_string(header_len, '\0');
            header_stream.read(&header_string[0], header_len);
            header = header_string;
      
            // std::istream is(&buf);
            // std::ostringstream oss;
            // oss << is.rdbuf();
            // buffer = oss.str();
      
            return header_len;
        } catch (std::exception& e) {
            std::cout << "Unable to read from socket: " << e.what() << std::endl;
            return 0;
        }
    }
    size_t read_exact(std::vector<uint8_t>& buffer, int new_size) {
        buffer = extract_remaining();
        int initial_size = buffer.size();
        buffer.resize(new_size);
        int read_size = new_size-initial_size;
        try {
            size_t len = boost::asio::read(socket, boost::asio::buffer(buffer.data() + initial_size, read_size));
            return len;
        }
        catch (std::exception& e) {
            std::cout << "Unable to read: " << e.what() << std::endl;
            return 0;
        }
    }
    size_t socket_read(std::vector<uint8_t>& buffer) {
        try {
            size_t len = socket.read_some(boost::asio::buffer(buffer));
            return len;
        } 
        catch (std::exception& e) {
            std::cout << "Unable to read from socket: " << e.what() << std::endl;
            return 0;
        }
    }
    void socket_write(const std::vector<uint8_t>& message) {
        try {
            boost::asio::write(socket, boost::asio::buffer(message));
        } 
        catch (std::exception& e) {
            std::cout << "Unable to write to socket: " << e.what() << std::endl;
        }
    }
private:
    boost::asio::ip::tcp::socket& socket;
    boost::asio::streambuf buf;
};

