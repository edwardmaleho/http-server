# http-server
An Asynchronous HTTP server that makes use of Boost.asio libraries and written in C++.

## Features
- Able to parse GET and POST requests for HTTP/1.1
- Supports Keep-Alive
- Able to handle multiple clients concurrently

Header Parsing was done manually (Not with Boost.Beast). It is not feature extensive but enough for simple embedded servers.

## Building
to build the server just run:
```bash
meson setup build
ninja -C build
```

## Dependencies
- Boost.Asio 1.66 or later (uses io_context)
- C++ compiler supporting C++17 or later

## Planned Features
- Add support for HTTPS using TLS (Boost.asio and OpenSSL).
