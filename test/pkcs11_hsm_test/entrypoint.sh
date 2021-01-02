#!/bin/bash

TOKEN="hsm"
PIN="plaintext"
KEY_LABEL="rsa-privkey"
CERT_LABEL="certchain"
CA_CERT_LABEL="ca-certificate"
CSR="CSR.csr"
LIB="/usr/lib/softhsm/libsofthsm2.so"
PUB="client.pub"

# Create Token
softhsm2-util --init-token --slot 0 --label "hsm" --so-pin ${PIN} --pin ${PIN}

# Get Token URL
URL=$(p11tool --provider ${LIB} --list-tokens | grep "URL: pkcs11:.*${TOKEN}" | cut -c 7-)

# Generate Keys
p11tool --provider ${LIB} --login --set-pin ${PIN} --generate-rsa --bits 2048 --label "${KEY_LABEL}" --outfile ${PUB} ${URL}

# Generate CSR
openssl req -engine pkcs11 -keyform engine -new -key ${URL} -out ${CSR} -subj "/C=US/O=Argo AI/OU=Security/CN=client"

# Validate CSR
openssl req -text -noout -verify -in ${CSR}

# Create the CA keys
openssl genrsa -out ca.key 2048

# Create the CA's self-signed cert
openssl req -new -x509 -days 3650 -key ca.key -out ca.crt -subj "/C=US/O=Argo AI/OU=Security/CN=Sample CA"

# Create the Server's keypair
openssl genrsa -out server.key 2048

# Create the CSR for the server
openssl req -new -key server.key -out server.req -sha256 -subj "/C=US/O=Argo AI/OU=Security/CN=localhost"

# Create the server's certificate from the provided CSR
openssl x509 -req -in server.req -CA ca.crt -CAkey ca.key -set_serial 100 -extensions server -days 800 -outform PEM -out server.crt -sha256

# Create the client's certificate from the provided CSR
openssl x509 -req -in ${CSR} -CA ca.crt -CAkey ca.key -set_serial 101 -extensions client -days 800 -outform PEM -out client.crt -sha256

# Display the certificate created in the previous step
openssl x509 -in client.crt -text -noout

# Import the certificate into the HSM
p11tool --provider ${LIB} --login --set-pin ${PIN} --label "${CERT_LABEL}" --write --load-certificate=client.crt ${URL}

# Import the CA certificate as trusted into the HSM
p11tool --provider ${LIB} --admin-login --set-so-pin ${PIN} --label "${CA_CERT_LABEL}" --write --load-certificate=ca.crt --trusted --ca ${URL}

pause 1

# Runs MQTT broken in background
mosquitto -c /app/mosquitto_sample.conf -v &

# Executes Paho with HSM
/app/paho_cs_pub topic -t test -c ssl://localhost:8883 --hsmmodule /lib/softhsm/libsofthsm2.so --cert client.crt --calabel my_cert --keylabel my_key --tokenlabel hsm --pin ${PIN}
