#!/bin/bash


if [ "$TRAVIS_OS_NAME" == "linux" ]; then
	sudo apt-get update -qq
	sudo apt-get install -y libssl-dev
fi

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
	brew update
	brew install openssl
	export CFLAGS="-I/usr/local/opt/openssl/include $CFLAGS" LDFLAGS="-L/usr/local/opt/openssl/lib $LDFLAGS"
fi
