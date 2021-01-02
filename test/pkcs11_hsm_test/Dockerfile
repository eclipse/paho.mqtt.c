FROM ubuntu:20.04

# Time needed for a dependency
RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime

# Dependencies
RUN apt-get -y update --fix-missing
RUN apt-get -y install softhsm libsofthsm2-dev build-essential gcc make cmake libssl-dev libp11-dev openssl vim git mosquitto gdb opensc gnutls-bin libengine-pkcs11-openssl
COPY openssl.cnf /etc/ssl/

# Workdir setup
COPY . /app
WORKDIR /app
RUN make
ENTRYPOINT ["/bin/bash"]
# Executes commands
CMD ["entrypoint.sh"]
