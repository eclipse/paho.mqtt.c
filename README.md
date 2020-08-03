[![Build Status](https://travis-ci.org/eclipse/paho.mqtt.c.svg?branch=master)](https://travis-ci.org/eclipse/paho.mqtt.c)
[![Total Alerts](https://img.shields.io/lgtm/alerts/g/eclipse/paho.mqtt.c.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/eclipse/paho.mqtt.c/alerts/)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/2339/badge.svg)](https://scan.coverity.com/projects/paho-c)

# Eclipse Paho C Client Library for the MQTT Protocol

This repository contains the source code for the [Eclipse Paho](http://eclipse.org/paho) MQTT C client library.

This code builds libraries which enable applications to connect to an [MQTT](http://mqtt.org) broker to publish messages, and to subscribe to topics and receive published messages.

Synchronous and various asynchronous programming models are supported.

## Information About MQTT

* [MQTT website](http://mqtt.org)
* [The MQTT 3.1.1 standard](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html)
* [The MQTT 5.0 standard](https://docs.oasis-open.org/mqtt/mqtt/v5.0/mqtt-v5.0.html)
* [HiveMQ introduction to MQTT](https://www.hivemq.com/mqtt/)
* [OASIS Introduction to MQTT presentation](https://www.oasis-open.org/committees/download.php/49205/MQTT-OASIS-Webinar.pdf)

## Libraries

The Paho C client comprises four variant libraries, shared or static:

 * paho-mqtt3a - asynchronous (MQTTAsync)
 * paho-mqtt3as - asynchronous with SSL (MQTTAsync)
 * paho-mqtt3c - "classic" / synchronous (MQTTClient)
 * paho-mqtt3cs - "classic" / synchronous with SSL (MQTTClient)

[Which Paho C API to use, with some history, for context](https://modelbasedtesting.co.uk/2013/10/13/which-paho-mqtt-c-api-to-use-and-some-history/)

## Usage and API

Detailed API documentation [is available online](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/index.html).  It is also available by building the Doxygen docs in the  ``doc`` directory. 

Samples are available in the Doxygen docs and also in ``src/samples`` for reference.  These are:

- paho_c_pub.c and paho_c_sub.c: command line utilities to publish and subscribe, -h will give help
- paho_cs_pub.c paho_cs_sub.c: command line utilities using MQTTClient to publish and subscribe
- MQTTClient_publish.c,	MQTTClient_subscribe.c and MQTTClient_publish_async.c: MQTTClient simple code examples
- MQTTAsync_publish.c and MQTTAsync_subscribe.c: MQTTAsync simple code examples						

Some potentially useful blog posts:

- [Paho client MQTT 5.0 support and command line utilities](https://modelbasedtesting.co.uk/2018/08/08/paho-c-client-mqtt-5-0-and-command-line-utilities/)
- [MQTT, QoS and persistence](https://modelbasedtesting.co.uk/2013/11/24/mqtt-qos-and-persistence/)
- [A story of MQTT 5.0](https://modelbasedtesting.co.uk/2018/04/09/a-story-of-mqtt-5-0/)

[Various MQTT and MQTT-SN talks I've given.](https://modelbasedtesting.co.uk/talks-ive-given/)

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

Please open issues in the Github project: https://github.com/eclipse/paho.mqtt.c/issues.

## More information

Discussion of the Paho clients takes place on the [Eclipse paho-dev mailing list](https://dev.eclipse.org/mailman/listinfo/paho-dev).

General questions about the MQTT protocol are discussed in the [MQTT Google Group](https://groups.google.com/forum/?hl=en-US&fromgroups#!forum/mqtt).

There is more information available via the [MQTT community site](http://mqtt.org).


## Build instructions for GNU Make

Ensure the OpenSSL development package is installed.  Then from the client library base directory run:

```
make
sudo make install
```

This will build and install the libraries.  To uninstall:

```
sudo make uninstall
```

To build the documentation requires doxygen and optionally graphviz.

```
make html
```

The provided GNU Makefile is intended to perform all build steps in the ```build``` directory within the source-tree of Eclipse Paho. Generated binares, libraries, and the documentation can be found in the ```build/output``` directory after completion. 

Options that are passed to the compiler/linker can be specified by typical Unix build variables:

Variable | Description
------------ | -------------
CC | Path to the C compiler
CFLAGS | Flags passed to compiler calls
LDFLAGS | Flags passed to linker calls


## Build requirements / compilation using CMake

The build process currently supports a number of Linux "flavors" including ARM and s390, OS X, AIX and Solaris as well as the Windows operating system. The build process requires the following tools:
  * CMake (http://cmake.org)
  * Ninja (https://martine.github.io/ninja/) or
    GNU Make (https://www.gnu.org/software/make/), and
  * gcc (https://gcc.gnu.org/).

On Debian based systems this would mean that the following packages have to be installed:

```
apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
```

Also, in order to build a debian package from the source code, the following packages have to be installed

```
apt-get install fakeroot fakeroot devscripts dh-make lsb-release
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
PAHO_BUILD_SHARED | TRUE | Build a shared version of the libraries
PAHO_BUILD_STATIC | FALSE | Build a static version of the libraries
PAHO_HIGH_PERFORMANCE | FALSE | When set to true, the debugging aids internal tracing and heap tracking are not included.
PAHO_WITH_SSL | FALSE | Flag that defines whether to build ssl-enabled binaries too. 
OPENSSL_ROOT_DIR | "" (system default) | Directory containing your OpenSSL installation (i.e. `/usr/local` when headers are in `/usr/local/include` and libraries are in `/usr/local/lib`)
PAHO_BUILD_DOCUMENTATION | FALSE | Create and install the HTML based API documentation (requires Doxygen)
PAHO_BUILD_SAMPLES | FALSE | Build sample programs
MQTT_TEST_BROKER | tcp://localhost:1883 | MQTT connection URL for a broker to use during test execution
MQTT_TEST_PROXY | tcp://localhost:1883 | Hostname of the test proxy to use
MQTT_SSL_HOSTNAME | localhost | Hostname of a test SSL MQTT broker to use
PAHO_BUILD_DEB_PACKAGE | FALSE | Build debian package

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


### Running the tests

Test code is available in the ``test`` directory. The tests can be built and executed with the CMake build system. The test execution requires a MQTT broker running. By default, the build system uses ```localhost```, however it is possible to configure the build to use an external broker. These parameters are documented in the Build Requirements section above.

After ensuring a MQTT broker is available, it is possible to execute the tests by starting the proxy and running `ctest` as described below:

```
python ../test/mqttsas2.py &
ctest -VV
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

## Microsoft Windows

### Calling convention

As is normal for C programs on Windows, the calling convention is __cdecl.  See the Microsoft documentation here:

https://docs.microsoft.com/en-us/cpp/cpp/cdecl?view=vs-2019

If you call this library from another language, you may need to take this into account.


