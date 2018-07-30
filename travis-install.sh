#!/bin/bash

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
