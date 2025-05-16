#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <sstream>
#include <fstream>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}

std::string http_response_msg(std::string response, std::string body, std::string type, size_t len) {
    std::string response_msg =  "HTTP/1.1 " + response + "\r\n"
                            "Date: Sun, 11 May 2025 13:15:00 GMT\r\n"
                            "Server: Myserver/2.4.41 (Ubuntu)\r\n"
                            "Content-Type: " + type + "\r\n"
                            "Content-Length: " + std::to_string(len) + "\r\n"
                            "Connection: keep-alive\r\n"
                            "\r\n" +
                            body +
                            "\r\n";
    return response_msg;
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

std::string read_file(std::string file_path) {
    std::ifstream file(file_path);
    if (!file) {
        return "";
    }
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

std::string process_request(std::string ip_addr, boost::array<char, 8224> buf, size_t len) {
    std::string data {buf.data(), len};
    std::stringstream stream (data);
    std::string method, target, http_type;
    std::string response;
    std::string body = "";
    std::string time = "Sun, 11 May 2025 13:15:00 GMT";
    size_t response_len = 0;
    std::string type;

    stream >> method >> target >> http_type;

    if (http_type != "HTTP/1.1") {
        response = "505 HTTP Version Not Supported";
    }
    if (method == "GET") {
        target.erase(0, 1);
        body = read_file(target);
        response_len = body.size();
        if (response_len == 0) {
            response = "404 File Not Found";
        } else {
            response = "200 OK";
        }
        if (target.find("html")) {
            type = "text/html";
        } else if (target.find("txt")) {
            type = "text/plain";
        }
    }
    std::string response_msg = http_response_msg(response, body, type, response_len);
    std::cout << response_msg;
    std::cout << "[" << ip_addr << "] " << " [" << time <<"] \"" << method << " " << target << " " << http_type << "\" " << response << std::endl;
    // std::string line;
    // while (std::getline(stream, line)) {
    //     if (!line.empty() && line.back() == '\r') {
    //         line.pop_back();
    //     }
    //     if (line.find("GET") != std::string::npos) {
    //         std::cout << line << std::endl;
    //     }
    // }
    // std::cout << data;
    return response_msg;
}

int main() {
    int port = 8080;
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            auto remote_endpoint = socket.remote_endpoint();
            std::string ip_addr = remote_endpoint.address().to_string();
        //     std::string message = "this is a message";
        //     boost::system::error_code ignored_error;
        //     boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        // }
        // while (true) {
            boost::array<char, 8224> buf;
            boost::system::error_code error;
            size_t len = read(socket, buf);
            std::string response_msg = process_request(ip_addr, buf, len);
            write(socket, response_msg);
            // std::cout << "read successfully" << std::endl;

            // if (error == boost::asio::error::eof)
            //     break;
            // else if (error)
            //     throw boost::system::system_error(error);
            // std::vector<char> msg;
            // std::cout.write(buf.data(), len);
            // std::cout << std::endl;
            // if (len == 0) {
            //     return 0;
            // }
         }
        
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
