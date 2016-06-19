# Eclipse Paho MQTT C client


This repository contains the source code for the [Eclipse Paho](http://eclipse.org/paho) MQTT C client library. 

This code builds libraries which enable applications to connect to an [MQTT](http://mqtt.org) broker to publish messages, and to subscribe to topics and receive published messages.

Both synchronous and asynchronous modes of operation are supported.

## Build requirements / compilation using CMake

There build process currently supports a number of Linux "flavors" including ARM and s390, OS X, AIX and Solaris as well as the Windows operating system. The build process requires the following tools:
  * CMake (http://cmake.org)
  * Ninja (https://martine.github.io/ninja/; preferred) or
    GNU Make (https://www.gnu.org/software/make/), and
  * gcc (https://gcc.gnu.org/).

On Debian based systems this would mean that the following packages have to be installed:

```
apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
```
Ninja can be downloaded from its github project page in the "releases" section. Optionally it is possible to build binaries with SSL support. This requires the OpenSSL libraries and includes to be available. E. g. on Debian:

```
apt-get install libssl-dev
```

The documentation requires doxygen and optionally graphviz:

```
apt-get install doxygen graphviz
```

Before compiling, determine the value of some variables in order to configure features, library locations, and other options:

Variable | Default Value | Description
------------ | ------------- | -------------
PAHO_WITH_SSL | FALSE | Flag that defines whether to build ssl-enabled binaries too. 
OPENSSL_INC_SEARCH_PATH | "" (system default) | Directory containing OpenSSL includes
OPENSSL_LIB_SEARCH_PATH | "" (system default) | Directory containing OpenSSL libraries
PAHO_BUILD_DOCUMENTATION | FALSE | Create and install the HTML based API documentation (requires Doxygen)
PAHO_BUILD_SAMPLES | FALSE | Build sample programs

Using these variables CMake can be used to generate your Ninja or Make files. Using CMake, building out-of-source is the default. Therefore it is recommended to invoke all build commands inside your chosen build directory but outside of the source tree.

An example build session targeting the build platform could look like this:

```
mkdir /tmp/build.paho
cd /tmp/build.paho
cmake -GNinja -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE ~/git/org.eclipse.paho.mqtt.c
```

Invoking cmake and specifying build options can also be performed using cmake-gui or ccmake (see https://cmake.org/runningcmake/). For example:

```
ccmake -GNinja ~/git/org.eclipse.paho.mqtt.c
```

To compile/link the binaries and to generate packages, simply invoke `ninja package` or `make -j <number-of-cores-to-use> package` after CMake. To simply compile/link invoke `ninja` or `make -j <number-of-cores-to-use>`. 

### Debug builds

Debug builds can be performed by defining the value of the ```CMAKE_BUILD_TYPE``` option to ```Debug```. For example:

```
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug git/org.eclipse.paho.mqtt.c
```

### Cross compilation

Cross compilation using CMake is performed by using so called "toolchain files" (see: http://www.vtk.org/Wiki/CMake_Cross_Compiling). 

The path to the toolchain file can be specified by using CMake's `-DCMAKE_TOOLCHAIN_FILE` option. In case no toolchain file is specified, the build is performed for the native build platform.

For your convenience toolchain files for the following platforms can be found in the `cmake` directory of Eclipse Paho:
  * Linux x86
  * Linux ARM11 (a.k.a. the Raspberry Pi)
  * Windows x86_64
  * Windows x86

The provided toolchain files assume that required compilers/linkers are to be found in the environment, i. e. the PATH-Variable of your user or system. If you prefer, you can also specify the absolute location of your compilers in the toolchain files. 

Example invocation for the Raspberry Pi:

```
cmake -GNinja -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_BUILD_DOCUMENTATION=TRUE -DOPENSSL_LIB_SEARCH_PATH=/tmp/libssl-dev/usr/lib/arm-linux-gnueabihf -DOPENSSL_INC_SEARCH_PATH="/tmp/libssl-dev/usr/include/openssl;/tmp/libssl-dev/usr/include/arm-linux-gnueabihf" -DCMAKE_TOOLCHAIN_FILE=~/git/org.eclipse.paho.mqtt.c/cmake/toolchain.linux-arm11.cmake ~/git/org.eclipse.paho.mqtt.c
```

Compilers for the Raspberry Pi can be obtained from e. g. Linaro (see: http://releases.linaro.org/15.06/components/toolchain/binaries/4.8/arm-linux-gnueabihf/). This example assumes that OpenSSL-libraries and includes have been installed in the ```/tmp/libssl-dev``` directory.

Example invocation for Windows 64 bit:

```
cmake -GNinja -DPAHO_BUILD_SAMPLES=TRUE -DCMAKE_TOOLCHAIN_FILE=~/git/org.eclipse.paho.mqtt.c/cmake/toolchain.win64.cmake ~/git/org.eclipse.paho.mqtt.c

```

In this case the libraries and executable are not linked against OpenSSL Libraries. Cross compilers for the Windows platform can be installed on Debian like systems like this:

```
apt-get install gcc-mingw-w64-x86-64 gcc-mingw-w64-i686
```


## Build instructions for GNU Make (deprecated)

The provided GNU Makefile is intended to perform all build steps in the ```build``` directory within the source-tree of Eclipse Paho. Generated binares, libraries, and the documentation can be found in the ```build/output``` directory after completion. 

Options that are passed to the compiler/linker can be specified by typical Unix build variables:

Variable | Description
------------ | -------------
CC | Path to the C compiler
CFLAGS | Flags passed to compiler calls
LDFLAGS | Flags passed to linker calls

## Libraries

The Paho C client comprises four shared libraries:

 * libmqttv3a.so - asynchronous
 * libmqttv3as.so - asynchronous with SSL
 * libmqttv3c.so - "classic" / synchronous
 * libmqttv3cs.so - "classic" / synchronous with SSL

## Usage and API

Detailed API documentation is available by building the Doxygen docs in the  ``doc`` directory. A [snapshot is also available online](http://www.eclipse.org/paho/files/mqttdoc/Cclient/index.html).

Samples are available in the Doxygen docs and also in ``src/samples`` for reference.

Note that using the C headers from a C++ program requires the following declaration as part of the C++ code:

```
    extern "C" {
    #include "MQTTClient.h"
    #include "MQTTClientPersistence.h"
    }
```

## Runtime tracing

A number of environment variables control runtime tracing of the C library. 

Tracing is switched on using ``MQTT_C_CLIENT_TRACE`` (a value of ON traces to stdout, any other value should specify a file to trace to). 

The verbosity of the output is controlled using the  ``MQTT_C_CLIENT_TRACE_LEVEL`` environment variable - valid values are ERROR, PROTOCOL, MINIMUM, MEDIUM and MAXIMUM (from least to most verbose).

The variable ``MQTT_C_CLIENT_TRACE_MAX_LINES`` limits the number of lines of trace that are output.

```
export MQTT_C_CLIENT_TRACE=ON
export MQTT_C_CLIENT_TRACE_LEVEL=PROTOCOL
```

## Reporting bugs

Please report bugs under the "MQTT-C" Component in [Eclipse Bugzilla](http://bugs.eclipse.org/bugs/) for the Paho Technology project.

## More information

Discussion of the Paho clients takes place on the [Eclipse paho-dev mailing list](https://dev.eclipse.org/mailman/listinfo/paho-dev).

General questions about the MQTT protocol are discussed in the [MQTT Google Group](https://groups.google.com/forum/?hl=en-US&fromgroups#!forum/mqtt).

There is much more information available via the [MQTT community site](http://mqtt.org).
