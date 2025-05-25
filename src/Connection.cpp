#include "Connection.hpp"

std::vector<uint8_t> Connection::extract_remaining() {
    auto data = buf.data();
    return std::vector<uint8_t>(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));
    
}

void Connection::async_read_until(std::function<void(const boost::system::error_code&, std::string)> handler) {
    auto self = shared_from_this();
    std::cout << "starting read\n";
   if (!socket || !socket->is_open()) {
        handler(boost::asio::error::not_connected, "");
        return;
    }
    boost::asio::async_read_until(*socket, buf, "\r\n\r\n", 
    [self, handler](const boost::system::error_code& ec, size_t header_len) {
        std::cout << "Inside handler\n";
        if (ec) {
            handler(ec, "");
            self->close();
            return; 
        }

        std::istream header_stream(&self->buf);
        std::string header_string(header_len, '\0');
        header_stream.read(&header_string[0], header_len);

        self->buf.consume(header_len);

        handler(ec, header_string);
    });
}

void Connection::async_read_exactly(int new_size, std::function<void(const boost::system::error_code&, std::vector<uint8_t>)> handler) {
    auto self = shared_from_this();
    std::vector<uint8_t> buffer = self->extract_remaining();
    std::size_t initial_size = buffer.size();
    buffer.resize(new_size);

    std::size_t read_size = new_size - initial_size;
    
    if (read_size == 0) {
        handler({}, buffer);
        return;
    }

    auto temp_buf = std::make_shared<std::vector<uint8_t>>(std::move(buffer));
    
    boost::asio::async_read(*socket, boost::asio::buffer(temp_buf->data() + initial_size, read_size), 
        [self, temp_buf, handler, read_size](const boost::system::error_code& ec, size_t bytes_transferred) mutable {
            if (ec) {
                handler(ec, {});
                self->close();
                return;
            }

            self->buf.consume(read_size);

            handler(ec, std::move(*temp_buf));
        }
    );
}

void Connection::async_write(const std::vector<uint8_t>& data, std::function<void(const boost::system::error_code&, size_t)> handler) {
    auto self = shared_from_this();
    boost::asio::async_write(*socket, boost::asio::buffer(data), 
        [self, handler](const boost::system::error_code& ec, size_t bytes_transferred) {
            handler(ec, bytes_transferred);
    });
}

void Connection::write_response_and_continue(HttpResponse response) {
    auto self = shared_from_this();
    self->async_write(response.to_bytes(), [self](const boost::system::error_code& ec, size_t) {
        std::cout << "Write completed" << std::endl;
        if (ec) {
            self->close();
            return;
        }
        if (self->is_open()) {
            if (self) {
                std::cout << "Starting with connection id: " << std::to_string(self->conn_id) << std::endl;
                self->session_loop();
            }
            else {
                std::cout << "Session loop is empty";
            }
        }
    });
};

void Connection::handle_header(std::string header) {
    auto self = shared_from_this();
    HttpRequest req;
    req.parse(header);
    std::cout << "Header:\n" << header << std::endl;

    if (req.headers.count("Content-Length")) {
        int body_len = std::stoul(req.headers.at("Content-Length"));

        self->async_read_exactly(body_len, [self, req](const boost::system::error_code& ec, std::vector<uint8_t> body) mutable {
            if (ec) {
                self->close();
                return;
            }
            req.body = std::move(body);
            HttpResponse response = self->handler.process_request(req);
            self->write_response_and_continue(response);
        });
    } else {
        HttpResponse response = self->handler.process_request(req);
        write_response_and_continue(response);
    }
};

void Connection::session_loop() {
    auto self = shared_from_this();
    std::cout << "This is a loop\n";
    self->async_read_until([self](const boost::system::error_code& ec, std::string header) {
        if (ec) {
            self->close();
            return;
        }
        self->handle_header(std::move(header));
    });
};

bool Connection::is_open() {
    if (socket->is_open()) {
        return true;
    }
    return false;
}

void Connection::close() {
    boost::system::error_code ec;

    if (socket->is_open()) {
        socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket->close(ec);
    }

    if (ec) {
        std::cerr << "Error closing socket: " << ec.message() << std::endl;
    }
}