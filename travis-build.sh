#!/bin/bash

set -e

rm -rf build.paho
mkdir build.paho
cd build.paho
echo "travis build dir $TRAVIS_BUILD_DIR pwd $PWD"
cmake -DCMAKE_BUILD_TYPE=Debug -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE ..
make
python3 ../test/mqttsas.py &
ctest -VV --timeout 600
cpack --verbose
kill %1
#killall mosquitto

