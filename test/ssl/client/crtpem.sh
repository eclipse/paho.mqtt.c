rm client.pem
openssl x509 -in ../server/ca.crt -text >> client.pem 
openssl x509 -in client.crt -text >> client.pem
openssl pkey -in client.key -text >> client.pem
