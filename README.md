# http-server (tls)
A lightweight, asynchronous HTTP/1.1 server written in modern C++ using the Boost.Asio networking library. It supports basic HTTP request handling over TLS, making it suitable for applications requiring simple and efficient HTTPS communication.

## Features
- Able to parse GET and POST requests for HTTP/1.1
- Supports Keep-Alive
- Able to handle multiple clients concurrently
- Self-signed certificates
- Minimal HTTP Header Parsing: Lightweight, custom implementation

## Building
to build the server just run:
```bash
meson setup build
ninja -C build
```

## Dependencies
- Boost.Asio 1.66 or later (uses io_context)
- C++ compiler supporting C++17 or later
- OpenSSL v3.0 recommended

## Certificate Generation
Use the provided script to generate the necessary TLS certificate and key:

```bash
./scripts/generate_certs.sh
```
This will create the following files in the ./certs/ directory:
- server.crt – Self-signed TLS certificate
- server.key – Private key
- server.csr – Certificate signing request (optional for external CA)
