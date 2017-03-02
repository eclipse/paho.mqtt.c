#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
  mkdir build.paho
  cd build.paho
  cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE ..
  make
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  make
fi
