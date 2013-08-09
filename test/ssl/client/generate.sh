
# Generate a client key.

    openssl genrsa -des3 -out client.key 2048

# Generate a certificate signing request to send to the CA.

    openssl req -out client.csr -key client.key -new

# Send the CSR to the CA, or sign it with your CA key:

    openssl x509 -req -in client.csr -CA ../server/ca.crt -CAkey ../server/ca.key -CAcreateserial -out client.crt -days 999999
