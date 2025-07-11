#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <boost/asio/ssl.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestHandler.hpp"

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream) : ssl_stream(std::move(ssl_stream)), handler("MyServer/1.0", "www") { }

    std::vector<uint8_t> extract_remaining();

    void start();

    void async_read_until(std::function<void(const boost::system::error_code&, std::string)> handler);
    void async_read_exactly(int new_size, std::function<void(const boost::system::error_code&, std::vector<uint8_t>)> handler);

    void async_write(const std::vector<uint8_t>& data, std::function<void(const boost::system::error_code&, size_t bytes_transferred)> handler);
    
    void write_response_and_continue(HttpResponse response);
    void handle_header(std::string header);
    void session_loop();
    
    // bool is_open();
    void close();

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream;
    boost::asio::streambuf buf;
    RequestHandler handler;
};

