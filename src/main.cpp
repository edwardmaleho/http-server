#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"
#include "Session.hpp"

void async_accept_loop(boost::asio::io_context& io_context, boost::asio::ip::tcp::acceptor& acceptor) {
    boost::asio::ip::tcp::socket socket(io_context);
    auto handler = std::make_shared<RequestHandler>("MyServer/1.0", "www");
    auto session = std::make_shared<Session>(std::move(socket));

    acceptor.async_accept(session->get_socket(), [session, &acceptor, &io_context, handler](const boost::system::error_code& ec) {
        if (!ec) {
            session->session_loop();
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