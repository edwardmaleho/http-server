#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"
#include "Connection.hpp"

void async_accept_loop(boost::asio::io_context& io_context, boost::asio::ip::tcp::acceptor& acceptor, int& conn_id) {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    auto handler = std::make_shared<RequestHandler>("MyServer/1.0", "www");

    acceptor.async_accept(*socket, [socket, &acceptor, &io_context, handler, &conn_id](const boost::system::error_code& ec) {
        if (!ec) {
            auto connection = std::make_shared<Connection>(socket, conn_id);
            connection->session_loop();
        };
        conn_id++;
        async_accept_loop(io_context, acceptor, conn_id);
    });
}

int main() {
    int port = 8081;
    int conn_id = 0;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

    async_accept_loop(io_context, acceptor, conn_id);

    io_context.run();

    return 0; 
}  