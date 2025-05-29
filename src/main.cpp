#include "HttpResponse.hpp"
#include "http_utils.hpp"
#include "HttpRequest.hpp"
#include <iostream>
#include "RequestHandler.hpp"
#include "Session.hpp"
#include <boost/asio/ssl.hpp>

void async_accept_loop(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::ssl::context& ssl_context) {
    auto handler = std::make_shared<RequestHandler>("MyServer/1.0", "www");

    acceptor.async_accept([&acceptor, &ssl_context, handler](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream(std::move(socket), ssl_context);
            std::make_shared<Session>(std::move(ssl_stream))->start();
        };
        async_accept_loop(acceptor, ssl_context);
    });
}

int main() {
    int port = 8080;
    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

    ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::single_dh_use);


    try {
        ssl_context.use_certificate_chain_file("certs/server.crt");
        ssl_context.use_private_key_file("certs/server.key", boost::asio::ssl::context::pem);
    }
    catch (boost::system::error_code& ec) {
        std::cout << "Unable to load certificate: " << ec.what() << std::endl;
        exit(1);
    }

    async_accept_loop(acceptor, ssl_context);

    io_context.run();

    return 0; 
}  