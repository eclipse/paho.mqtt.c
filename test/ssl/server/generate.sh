

#openssl req -new -x509 -days 999999 -extensions v3_ca -keyout ca.key -out ca.crt

openssl genrsa -des3 -out server.key 2048

openssl req -out server.csr -key server.key -new

openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 999999
