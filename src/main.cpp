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
            
            std::function<void()> session_loop;

            auto write_response_and_continue = [connection, &session_loop](HttpResponse response) {
                connection->async_write(response.to_bytes(), [connection, &session_loop](const boost::system::error_code& ec, size_t) {
                    if (ec) {
                        connection->close();
                        return;
                    }
                    if (connection->is_open()) {
                        session_loop();
                    }
                });
            };

            std::function<void(std::string)> handle_header = [connection, handler, &session_loop, write_response_and_continue](std::string header) {
                HttpRequest req;
                req.parse(header);
                std::cout << "Header:\n" << header << std::endl;

                if (req.headers.count("Content-Length")) {
                    int body_len = std::stoul(req.headers.at("Content-Length"));

                    connection->async_read_exactly(body_len, [connection, handler, req = std::move(req), write_response_and_continue](const boost::system::error_code& ec, std::vector<uint8_t> body) mutable {
                        if (ec) {
                            connection->close();
                            return;
                        }
                        req.body = std::move(body);
                        HttpResponse response = handler->process_request(req);
                        write_response_and_continue(response);
                    });
                } else {
                    HttpResponse response = handler->process_request(req);
                    write_response_and_continue(response);
                }
            };

            session_loop = [connection, handle_header]() {
                std::cout << "This is a loop\n";
                connection->async_read_until([connection, handle_header](const boost::system::error_code& ec, std::string header) {
                    if (ec) {
                        connection->close();
                        return;
                    }
                    handle_header(std::move(header));
                });
            };

            session_loop();

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