#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

std::string http_request_msg(std::string type, std::string url) {
    std::string request =  type + " /index.html HTTP/1.1\r\n"
                           "Host: " + url + "\r\n"
                           "User-Agent: MyClient/1.0\r\n"
                           "Accept: */*\r\n"
                           "Connection: close\r\n"
                           "\r\n";
    return request;
}

void connect(boost::asio::io_context& io_context, tcp::socket& socket, std::string host, std::string port) {
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    try {
        boost::asio::connect(socket, endpoints);
    } 
    catch(const boost::system::system_error& e) {
        std::cout << "Failed to connect" << std::endl;
    }
}
size_t read(tcp::socket& socket, boost::array<char, 8224>& buf) {
    boost::system::error_code error;
    try {
        size_t len = socket.read_some(boost::asio::buffer(buf), error);
        return len;
    } 

    catch (boost::system::error_code& e) {
        std::cout << e.what() << std::endl;
    }
}
void write(tcp::socket& socket, std::string message) {
    boost::system::error_code error;
    try {
        boost::asio::write(socket, boost::asio::buffer(message), error);
    } 
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        connect(io_context, socket, "localhost", "8080");        
        std::string request = http_request_msg("GET", "localhost::8080/index.html");
        boost::system::error_code write_error;
        write(socket, request);
        if (write_error) {
            std::cout << "Error: " << write_error.message() << std::endl;
        }
        while (true) {
            boost::array<char, 8224> buf;
            boost::system::error_code error;
            size_t len = read(socket, buf);
            // std::cout << "read successfully" << std::endl;

            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);
            std::vector<char> msg;
            std::cout.write(buf.data(), len);
            std::cout << std::endl;
            if (len == 0) {
                return 0;
            }
         }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}