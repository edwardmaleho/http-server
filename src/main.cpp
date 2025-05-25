#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"
#include "Connection.hpp"

void async_accept_loop(boost::asio::io_context& io_context, boost::asio::ip::tcp::acceptor& acceptor) {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    auto handler = std::make_shared<RequestHandler>("MyServer/1.0", "www");

    acceptor.async_accept(*socket, [socket, &acceptor, &io_context, handler](const boost::system::error_code& ec) {
        if (!ec) {
            auto connection = std::make_shared<Connection>(socket);
            connection->session_loop();
        };
        async_accept_loop(io_context, acceptor);
    });
}

int main() {
    int port = 8081;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

    async_accept_loop(io_context, acceptor);

    io_context.run();

    return 0; 
}  