#!/bin/bash

CERT_DIR="$(realpath $(dirname $0)/../certs)"
CERT="$CERT_DIR/server.crt"
KEY="$CERT_DIR/server.key"
CSR="$CERT_DIR/server.csr"

echo "Generating self-signed Certificate for server:"
mkdir -p $CERT_DIR

echo "Generating key..."
openssl genrsa -out "$KEY" 2048

echo "Generating Certificate signing request:"
openssl req -new -key "$KEY" -out "$CSR"

echo "Self signing certificate:"
openssl x509 -req -days 365 -in "$CSR -signkey "$KEY" -out "$CERT"


