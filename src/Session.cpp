#include "Session.hpp"

std::vector<uint8_t> Session::extract_remaining() {
    auto data = buf.data();
    return std::vector<uint8_t>(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));
    
}

void Session::start() {
    auto self = shared_from_this();
    ssl_stream.async_handshake(boost::asio::ssl::stream_base::server, [self] (const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "Handshake successful\n";
            self->session_loop();    
        }
        else {
            std::cout << "Handshake unsuccessful: " << ec.what() << std::endl;
        }
    });
}

void Session::async_read_until(std::function<void(const boost::system::error_code&, std::string)> handler) {
    auto self = shared_from_this();
    std::cout << "Starting read\n";
    boost::asio::async_read_until(ssl_stream, buf, "\r\n\r\n", 
    [self, handler](const boost::system::error_code& ec, size_t header_len) {
        if (ec) {
            handler(ec, "");
            std::cout << "Error: " << ec.what() << std::endl;
            self->close();
            return; 
        }
        std::cout << "Reading header\n";
        std::istream header_stream(&self->buf);
        std::string header_string(header_len, '\0');
        header_stream.read(&header_string[0], header_len);

        self->buf.consume(header_len);

        handler(ec, header_string);
    });
}

void Session::async_read_exactly(int new_size, std::function<void(const boost::system::error_code&, std::vector<uint8_t>)> handler) {
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
    
    std::cout << "Reading body\n";

    boost::asio::async_read(ssl_stream, boost::asio::buffer(temp_buf->data() + initial_size, read_size), 
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

void Session::async_write(const std::vector<uint8_t>& data, std::function<void(const boost::system::error_code&, size_t)> handler) {
    auto self = shared_from_this();
    std::cout << "Writing to socket\n";
    boost::asio::async_write(ssl_stream, boost::asio::buffer(data), 
        [self, handler](const boost::system::error_code& ec, size_t bytes_transferred) {
            handler(ec, bytes_transferred);
    });
}

void Session::write_response_and_continue(HttpResponse response) {
    auto self = shared_from_this();
    self->async_write(response.to_bytes(), [self](const boost::system::error_code& ec, size_t) {
        if (ec) {
            self->close();
            return;
        }
        if (self) {
            self->session_loop();
        }
        else {
            std::cout << "Session loop is empty";
        }
    });
};

void Session::handle_header(std::string header) {
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

void Session::session_loop() {
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

void Session::close() {
    ssl_stream.shutdown();
}