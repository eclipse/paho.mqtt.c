#!/bin/bash

if [ "$ENABLE_MBEDTLS" == "yes" ]; then
	git clone --depth 5 --branch mbedtls-2.13.1 https://github.com/ARMmbed/mbedtls
	pushd mbedtls
	cmake -DUSE_SHARED_MBEDTLS_LIBRARY:BOOL=ON -DUSE_STATIC_MBEDTLS_LIBRARY:BOOL=OFF .
	make
	sudo make install
	popd
fi

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	pwd
	sudo service mosquitto stop
	# Stop any mosquitto instance which may be still running from previous runs
	killall mosquitto
	# mosquitto -h
	# mosquitto -c test/tls-testing/mosquitto.conf &

	git clone https://github.com/eclipse/paho.mqtt.testing.git
	cd paho.mqtt.testing/interoperability
	python3 startbroker.py -c localhost_testing.conf &
	cd ../..
fi

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
	pwd
	brew update
	# brew install openssl mosquitto
	# brew services stop mosquitto
	# /usr/local/sbin/mosquitto -c test/tls-testing/mosquitto.conf &

	brew upgrade python
	git clone https://github.com/eclipse/paho.mqtt.testing.git
	cd paho.mqtt.testing/interoperability
	python3 startbroker.py -c localhost_testing.conf &
	cd ../..
fi
