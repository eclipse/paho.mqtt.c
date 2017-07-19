#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
  ./bootstrap && rm -rf build.paho.autotools/ && mkdir build.paho.autotools && pushd build.paho.autotools && ../configure --enable-samples --disable-doc --without-ssl && make && popd ..

  mkdir build.paho
  cd build.paho
  echo "travis build dir $TRAVIS_BUILD_DIR pwd $PWD"
  cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE ..
  make
  python ../test/mqttsas2.py &
  ctest -VV --timeout 600
  kill %1
  killall mosquitto
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  ./bootstrap && rm -rf build.paho.autotools/ && mkdir build.paho.autotools && pushd build.paho.autotools && ../configure --enable-samples --enable-doc --with-ssl && make && popd ..

  mkdir build.paho
  cd build.paho
  echo "travis build dir $TRAVIS_BUILD_DIR pwd $PWD"
  cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE ..
  make
  python ../test/mqttsas2.py &
  ctest -VV --timeout 600
  kill %1
  killall mosquitto
fi
