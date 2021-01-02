# Runs MQTT broken in background
mosquitto -c test/pkcs11_hsm_test/mosquitto_sample.conf -v &

# Executes Paho with HSM
paho_cs_pub topic -t test -c ssl://localhost:8883 --hsmmodule /lib/softhsm/libsofthsm2.so --cert test/certs/client/client.crt --calabel my_cert --keylabel my_key --tokenlabel hsm --pin abcdefg
