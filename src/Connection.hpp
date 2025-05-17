#pragma once

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

size_t socket_read(tcp::socket& socket, std::vector<uint8_t>& buffer, boost::system::error_code& error) {
    error.clear();
    try {
        size_t len = socket.read_some(boost::asio::buffer(buffer), error);
        return len;
    } 

    catch (boost::system::error_code& e) {
        std::cout << e.what() << std::endl;
    }
}

void socket_write(tcp::socket& socket, std::string message, boost::system::error_code& error) {
    error.clear();
    try {
        boost::asio::write(socket, boost::asio::buffer(message), error);
    } 
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}


// --- Socket I/O Wrappers (could be part of a Connection class) ---
/**
 * @brief Reads data from the socket.
 * @param socket The TCP socket.
 * @param buffer The buffer to store read data.
 * @param error Output error code.
 * @return Number of bytes read.
 */
size_t socket_read(boost::asio::ip::tcp::socket& socket, boost::asio::mutable_buffer buffer, boost::system::error_code& error) {
    error.clear(); // Clear error code before operation
    size_t length = 0;
    try {
        length = socket.read_some(buffer, error);
    } catch (const std::exception& e) {
        std::cerr << "Exception during socket read: " << e.what() << std::endl;
        // If ASIO didn't set an error but an exception occurred,
        // we might want to set a generic error or rethrow.
        if (!error) {
            // This is a bit tricky. If read_some throws, error might not be set.
            // For now, we rely on the `error` parameter set by read_some itself.
            // If an exception is thrown by ASIO that isn't a system_error, `error` might not be informative.
        }
        return 0; // Indicate failure or no bytes read
    }
    return length;
}

/**
 * @brief Writes data to the socket.
 * @param socket The TCP socket.
 * @param message The string message to write.
 * @param error Output error code.
 */
void socket_write(boost::asio::ip::tcp::socket& socket, const std::string& message, boost::system::error_code& error) {
    error.clear(); // Clear error code before operation
    try {
        boost::asio::write(socket, boost::asio::buffer(message), error);
    } catch (const std::exception& e) {
        std::cerr << "Exception during socket write: " << e.what() << std::endl;
        if (!error) {
            // Similar to read, ensure error reflects the failure if possible.
        }
    }
}

