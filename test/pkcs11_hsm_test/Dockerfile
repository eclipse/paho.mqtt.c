FROM ubuntu:20.04

# Time needed for a dependency
RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime

# Dependencies
RUN apt-get -y update --fix-missing
RUN apt-get -y install softhsm libsofthsm2 build-essential gcc make cmake libssl-dev libp11-dev openssl vim git mosquitto gdb opensc

# Workdir setup
COPY . /app
WORKDIR /app

# Import HSM
RUN softhsm2-util --init-token --slot 0 --label "hsm" --so-pin abcdefg --pin abcdefg
RUN softhsm2-util --pin abcdefg --import test/certs/client/client.p8 --token hsm --id a000 --label my_key
RUN pkcs11-tool --module /lib/softhsm/libsofthsm2.so -l --id a001 --label my_cert -y cert -w test/certs/ca/ca.crt -p abcdefg --token hsm

RUN make
RUN make install

ENTRYPOINT ["/bin/bash"]

# Executes commands
CMD ["test/pkcs11_hsm_test/entrypoint.sh"]
