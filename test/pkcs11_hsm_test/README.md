# Testing Guide

For the easy testing, some tools and key materials are provided in this directory.

## Key materials
------

```
ca.crt : CA certificate for both the client and server certificates. 
ca.key : Private key to sign the CA certificate
client.crt : Client certificate (for Paho MQTT publisher and subscriber)
client.key : Private key to sign the client certificate
server.crt : Server certificate (for Mosquitto MQTT Client)
server.key : Private key to sign the server certificate
```

## Testing with Docker
------

Using the Dockerfile found [here](./Dockerfile) you can test the PKCS11 extension with SoftHSM and sample certificates. 

The first step is to switch on the `PKCS11_ENABLED` flag in the [Makefile](../../Makefile):
```
# PKCS11_HSM Configuration
# - the following flags should be adjusted to build with PKCS11_HSM support
# - leaving blank will disable PKC11_HSM support
PKCS11_ENABLED = ON
#
```

Next, building docker from the repository root directory and subsequently running with the commands:

```
docker build -t paho_pkcs11_extension -f test/pkcs11_hsm_test/Dockerfile .
docker run paho_pkcs11_extension
```

The expected output should be similar to:

```
1599970065: mosquitto version 1.6.9 starting
1599970065: Config loaded from test/pkcs11_hsm_test/mosquitto_sample.conf.
1599970065: Opening ipv4 listen socket on port 8883.
1599970065: Opening ipv6 listen socket on port 8883.
1599970065: New connection from 127.0.0.1 on port 8883.
1599970066: New client connected from 127.0.0.1 as paho-cs-pub (p2, c1, k10).
1599970066: No will message specified.
1599970066: Sending CONNACK to paho-cs-pub (0, 0)
1599970066: Received DISCONNECT from paho-cs-pub
1599970066: Client paho-cs-pub disconnected.
<DEBUG> private key found: label(my_key)
<DEBUG> certificate found: label(my_cert)
```

## Testing with SoftHSM
------

Without a real HSM, the sample code can be also tested with SoftHSM. To use the SoftHSM, the following packages need to be installed.

```
sudo apt-get install softhsm libsofthsm2 opensc
```

In order to insert the key materials to the SoftHSM, the token should be initialized.

```
softhsm2-util --init-token --free --label <token_label> --pin <user_pin> --so-pin <security_officer_pin>
```

The RSA key pair can be generated within the SoftHSM by using pkcs11-tool. Alternatively sample keys and certificates can be found [here](./certs).

```
pkcs11-tool --module <path_of_libsofthsm2.so> -l -k --key-type rsa:2048 --id <key_object_id> --label <key_label> --token-label <token_label>
```

By using the private key stored in the SoftHSM, the X509 cerfiticate can be generated. Here, the openssl.cnf file should be configure to access the pkcs11 library. The example openssl.cnf file is located in this directory and the environment variable can be set to refer to the configuration file like `export OPENSSL_CONF=<file_path>`

```
openssl req -new -out <cert_signing_req_file_name> -engine pkcs11 -keyform engine -key "pkcs11:type=private;object=<key_label>;token=<token_label>;pin-value=<pin-value>"   # generate a certificate signing request (CSR)
openssl x509 -req -in <cer_signing_req_file_name> -CA <CA_file_path> -CAkey <CA_key_file_path> -CAcreateserial -out <cert_file_name>    # generate a certificate from the CSR

```

The CA certificate should also be stored in the SoftHSM. This can be done by using pkcs11-tool.

```
pkcs11-tool --module <path_of_libsofthsm2.so> -l --id <cert_object_id> --label <cert_label> -y cert -w <cert_file_path> -p <user_pin> --token <token_label>
```

After inserting all key materials, the content of SoftHSM can be checked by the provided tool. In this directory, use 'make' to build a small program called 'libp11_readhsm'. Please make sure the following lines in 'Makefile' should be edited with the proper paths. The user_pin needs to be configured in the source code (libp11_readhsm.c) before the build.

The excution of the program shows the whole content of SoftHSM as follows.

```
$ ./libp11_readhsm
4 number of slots are found!
#1 token: label(yk_token1), serial(499d226f1c6b7177), login-required(0x1)
  #1 certificate: label(yk_cert1), id(0x1000), x509(0x55a9a9572a30)
  #1 public key: label(yk_key1), id(0x1001), login(0), evp_key(0x55a9a9572380)
  #1 private key: label(yk_key1), id(0x1001), login(0), evp_key(0x55a9a9573af0)
#2 token: label(yk_token2), serial(994434b0281dcdae), login-required(0x1)
  #1 certificate: label(ca_cert), id(0x2002), x509(0x55a9a9575570)
  #2 certificate: label(yk_cert2), id(0x2000), x509(0x55a9a9576a40)
  #1 public key: label(client_key), id(0x2003), login(0), evp_key(0x55a9a95751b0)
  #2 public key: label(yk_key2), id(0x2001), login(0), evp_key(0x55a9a9577d40)
  #1 private key: label(client_key), id(0x2003), login(0), evp_key(0x55a9a9578ed0)
  #2 private key: label(yk_key2), id(0x2001), login(0), evp_key(0x55a9a9579240)
#3 token: label(yk_token1), serial(4d8f26b13e5438a8), login-required(0x1)
  #1 certificate: label(yk_cert3), id(0x3000), x509(0x55a9a95797d0)
  #2 certificate: label(yk_cert3), id(0x3001), x509(0x55a9a957a6d0)
  #1 public key: label(yk_key3), id(0x3002), login(0), evp_key(0x55a9a957a3d0)
  #2 public key: label(yk_key3), id(0x3003), login(0), evp_key(0x55a9a957bab0)
  #1 private key: label(yk_key3), id(0x3002), login(0), evp_key(0x55a9a957cd40)
  #2 private key: label(yk_key3), id(0x3003), login(0), evp_key(0x55a9a957a2c0)
no token!
authentication successfull.

```

## MQTT over TLS session test between Paho client and Mosquitto server
------

With all the key materials prepared, the MQTT over TLS session can be tested. First, the Mosquitto server should be run. For the easy configuration, the sample configuration file (mosquitto_sample.conf) is included in this directory.

```
$ ./mosquitto -c ./mosquitto_sample.conf -v
1594363880: mosquitto version 1.6.10 starting
1594363880: Config loaded from ../mosquitto.conf.
1594363880: Opening ipv4 listen socket on port 8883.
1594363880: Opening ipv6 listen socket on port 8883.

```

After loading the mosquitto server, the paho client can be run as follows.

```
$ ./paho_cs_pub topic -t test -c ssl://localhost:8883 --hsmmodule /usr/lib/softhsm/libsofthsm2.so --cert ./certs/client_hsm.crt --calabel ca_cert --keylabel client_key --tokenlabel yk_token2 --pin yksecret2
<DEBUG> private key found: label(client_key)
<DEBUG> certificate found: label(ca_cert)
```

When the client successfully connected to the server, the mosquitto server will print the following message.

```
$ ./mosquitto -c ../mosquitto_sample.conf -v
1594363880: mosquitto version 1.6.10 starting
1594363880: Config loaded from ../mosquitto.conf.
1594363880: Opening ipv4 listen socket on port 8883.
1594363880: Opening ipv6 listen socket on port 8883.
1594363888: New connection from 127.0.0.1 on port 8883.
1594363888: New client connected from 127.0.0.1 as paho-cs-pub (p2, c1, k10, u'localhost').
1594363888: No will message specified.
1594363888: Sending CONNACK to paho-cs-pub (0, 0)
1594363899: Received PINGREQ from paho-cs-pub
1594363899: Sending PINGRESP to paho-cs-pub
1594363909: Received PINGREQ from paho-cs-pub
1594363909: Sending PINGRESP to paho-cs-pub
1594363919: Received PINGREQ from paho-cs-pub
1594363919: Sending PINGRESP to paho-cs-pub
1594363930: Received PINGREQ from paho-cs-pub
```
