#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Platform parameter is missing."
    exit 1
fi

if [ -z ${BUILD_DIR+x} ]
then
    BUILD_DIR=/tmp/paho-build
fi

export PATH=${PATH}:/usr/local/arm-linux-gnueabihf/bin

cd ${BUILD_DIR}
if [ -d paho.mqtt.c ]
then
    (cd paho.mqtt.c && git pull origin master)
else
    git clone https://github.com/rpoisel/paho.mqtt.c.git
fi
if [ ! -f paho.mqtt.c/cmake/toolchain.$1.cmake ]
then
    echo "Platform $1 is not supported."
    exit 2
fi

rm -rf ${BUILD_DIR}/build-$1 >/dev/null 2>&1
mkdir -p ${BUILD_DIR}/build-$1
cd ${BUILD_DIR}/build-$1
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=../paho.mqtt.c/cmake/toolchain.$1.cmake ../paho.mqtt.c
ninja -v package

exit 0
