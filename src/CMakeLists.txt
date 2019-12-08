#*******************************************************************************
#  Copyright (c) 2015, 2018 logi.cals GmbH and others
#
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v1.0
#  and Eclipse Distribution License v1.0 which accompany this distribution.
#
#  The Eclipse Public License is available at
#     http://www.eclipse.org/legal/epl-v10.html
#  and the Eclipse Distribution License is available at
#    http://www.eclipse.org/org/documents/edl-v10.php.
#
#  Contributors:
#     Rainer Poisel - initial version
#     Ian Craggs (IBM Corp.) - merge master
#     Ian Craggs - update for MQTTV5 support
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

## compilation/linkage settings
INCLUDE_DIRECTORIES(
    .
    ${CMAKE_BINARY_DIR}
    )

CONFIGURE_FILE(VersionInfo.h.in
    ${CMAKE_BINARY_DIR}/VersionInfo.h
    @ONLY
    )

SET(common_src
    MQTTProtocolClient.c
    Clients.c
    utf-8.c
    StackTrace.c
    MQTTPacket.c
    MQTTPacketOut.c
    Messages.c
    Tree.c
    Socket.c
    Log.c
    MQTTPersistence.c
    Thread.c
    MQTTProtocolOut.c
    MQTTPersistenceDefault.c
    SocketBuffer.c
    Heap.c
    LinkedList.c
    MQTTProperties.c
    MQTTReasonCodes.c
    Base64.c
    SHA1.c
    WebSocket.c
    )

IF (WIN32)
    SET(LIBS_SYSTEM ws2_32)
ELSEIF (UNIX)
    IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
        SET(LIBS_SYSTEM c dl pthread)
    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Android")
        SET(LIBS_SYSTEM c dl)
    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        SET(LIBS_SYSTEM compat pthread)
    ELSE()
        SET(LIBS_SYSTEM c pthread)
    ENDIF()
ENDIF()

## common compilation for libpaho-mqtt3c and libpaho-mqtt3a
ADD_LIBRARY(common_obj OBJECT ${common_src})
SET_PROPERTY(TARGET common_obj PROPERTY POSITION_INDEPENDENT_CODE ON)

ADD_EXECUTABLE(MQTTVersion MQTTVersion.c)

ADD_LIBRARY(paho-mqtt3c SHARED $<TARGET_OBJECTS:common_obj> MQTTClient.c)
ADD_LIBRARY(paho-mqtt3a SHARED $<TARGET_OBJECTS:common_obj> MQTTAsync.c)

TARGET_LINK_LIBRARIES(paho-mqtt3c ${LIBS_SYSTEM})
TARGET_LINK_LIBRARIES(paho-mqtt3a ${LIBS_SYSTEM})

TARGET_LINK_LIBRARIES(MQTTVersion paho-mqtt3a paho-mqtt3c ${LIBS_SYSTEM})
SET_TARGET_PROPERTIES(
    paho-mqtt3c paho-mqtt3a PROPERTIES
    VERSION ${CLIENT_VERSION}
    SOVERSION ${PAHO_VERSION_MAJOR})

INSTALL(TARGETS paho-mqtt3c paho-mqtt3a
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(TARGETS MQTTVersion
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

IF (PAHO_BUILD_STATIC)
    ADD_LIBRARY(paho-mqtt3c-static STATIC $<TARGET_OBJECTS:common_obj> MQTTClient.c)
    ADD_LIBRARY(paho-mqtt3a-static STATIC $<TARGET_OBJECTS:common_obj> MQTTAsync.c)

    TARGET_LINK_LIBRARIES(paho-mqtt3c-static ${LIBS_SYSTEM})
    TARGET_LINK_LIBRARIES(paho-mqtt3a-static ${LIBS_SYSTEM})
    SET_TARGET_PROPERTIES(paho-mqtt3c-static PROPERTIES OUTPUT_NAME paho-mqtt3c)
    SET_TARGET_PROPERTIES(paho-mqtt3a-static PROPERTIES OUTPUT_NAME paho-mqtt3a)

    INSTALL(TARGETS paho-mqtt3c-static paho-mqtt3a-static
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
ENDIF()

INSTALL(FILES MQTTAsync.h MQTTClient.h MQTTClientPersistence.h MQTTProperties.h MQTTReasonCodes.h MQTTSubscribeOpts.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

IF (PAHO_WITH_SSL)
    SET(OPENSSL_SEARCH_PATH "" CACHE PATH "Directory containing OpenSSL libraries and includes")

    IF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
      SET(OPENSSL_SEARCH_PATH "/usr/local/opt/openssl")
    ENDIF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

    IF (WIN32)
      SET(OPENSSL_SEARCH_PATH "C:/OpenSSL-Win64")
      SET(OPENSSL_SEARCH_LIB_PATH "${OPENSSL_SEARCH_PATH}/lib64")
      IF (DEFINED ENV{PAHO_WINDOWS_BUILD_BIT})
        IF ($ENV{PAHO_WINDOWS_BUILD_BIT} STREQUAL "x86")
          SET(OPENSSL_SEARCH_PATH "C:/OpenSSL-Win32")
          SET(OPENSSL_SEARCH_LIB_PATH "${OPENSSL_SEARCH_PATH}/lib32")
        ENDIF ()
      ENDIF ()
    ELSE()
      SET(OPENSSL_SEARCH_LIB_PATH "${OPENSSL_SEARCH_PATH}/lib64")
    ENDIF ()

    FIND_PATH(OPENSSL_INCLUDE_DIR openssl/ssl.h
        HINTS ${OPENSSL_SEARCH_PATH}/include)
    FIND_LIBRARY(OPENSSL_LIB NAMES ssl libssl ssleay32
        HINTS ${OPENSSL_SEARCH_PATH}/lib ${OPENSSL_SEARCH_LIB_PATH})
    FIND_LIBRARY(OPENSSLCRYPTO_LIB NAMES crypto libcrypto libeay32
      	HINTS ${OPENSSL_SEARCH_PATH}/lib ${OPENSSL_SEARCH_LIB_PATH})

    MESSAGE(STATUS "OpenSSL hints: ${OPENSSL_SEARCH_PATH}")
    MESSAGE(STATUS "OpenSSL headers found at ${OPENSSL_INCLUDE_DIR}")
    MESSAGE(STATUS "OpenSSL library found at ${OPENSSL_LIB}")
    MESSAGE(STATUS "OpenSSL Crypto library found at ${OPENSSLCRYPTO_LIB}")

    INCLUDE_DIRECTORIES(
        ${OPENSSL_INCLUDE_DIR}
    )

    ## common compilation for libpaho-mqtt3cs and libpaho-mqtt3as
    ## Note: SSL libraries must be recompiled due ifdefs
    ADD_LIBRARY(common_ssl_obj OBJECT ${common_src})
    SET_PROPERTY(TARGET common_ssl_obj PROPERTY POSITION_INDEPENDENT_CODE ON)
    SET_PROPERTY(TARGET common_ssl_obj PROPERTY COMPILE_DEFINITIONS "OPENSSL=1")
    ADD_LIBRARY(paho-mqtt3cs SHARED $<TARGET_OBJECTS:common_ssl_obj> MQTTClient.c SSLSocket.c)
    ADD_LIBRARY(paho-mqtt3as SHARED $<TARGET_OBJECTS:common_ssl_obj> MQTTAsync.c SSLSocket.c)

    TARGET_LINK_LIBRARIES(paho-mqtt3cs ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} ${LIBS_SYSTEM})
    TARGET_LINK_LIBRARIES(paho-mqtt3as ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} ${LIBS_SYSTEM})
    SET_TARGET_PROPERTIES(
        paho-mqtt3cs paho-mqtt3as PROPERTIES
        VERSION ${CLIENT_VERSION}
        SOVERSION ${PAHO_VERSION_MAJOR}
        COMPILE_DEFINITIONS "OPENSSL=1")
    INSTALL(TARGETS paho-mqtt3cs paho-mqtt3as
        ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR})

    IF (PAHO_BUILD_STATIC)
        ADD_LIBRARY(paho-mqtt3cs-static STATIC $<TARGET_OBJECTS:common_ssl_obj> MQTTClient.c SSLSocket.c)
        ADD_LIBRARY(paho-mqtt3as-static STATIC $<TARGET_OBJECTS:common_ssl_obj> MQTTAsync.c SSLSocket.c)

        TARGET_LINK_LIBRARIES(paho-mqtt3cs-static ${OPENSSL_LIBRARIES} ${LIBS_SYSTEM})
        TARGET_LINK_LIBRARIES(paho-mqtt3as-static ${OPENSSL_LIBRARIES} ${LIBS_SYSTEM})
        SET_TARGET_PROPERTIES(
        paho-mqtt3cs-static paho-mqtt3as-static PROPERTIES
        VERSION ${CLIENT_VERSION}
        SOVERSION ${PAHO_VERSION_MAJOR}
        COMPILE_DEFINITIONS "OPENSSL=1")
        SET_TARGET_PROPERTIES(paho-mqtt3cs-static PROPERTIES OUTPUT_NAME paho-mqtt3cs)
        SET_TARGET_PROPERTIES(paho-mqtt3as-static PROPERTIES OUTPUT_NAME paho-mqtt3as)

        INSTALL(TARGETS paho-mqtt3cs-static paho-mqtt3as-static
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    ENDIF()
ENDIF()

# Base64 test
ADD_EXECUTABLE( Base64Test EXCLUDE_FROM_ALL Base64.c Base64.h )
TARGET_COMPILE_DEFINITIONS( Base64Test PUBLIC "-DBASE64_TEST" )
IF (PAHO_WITH_SSL)
	ADD_EXECUTABLE( Base64TestOpenSSL EXCLUDE_FROM_ALL Base64.c Base64.h )
	TARGET_LINK_LIBRARIES( Base64TestOpenSSL ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} )
	TARGET_COMPILE_DEFINITIONS( Base64TestOpenSSL PUBLIC "-DBASE64_TEST -DOPENSSL=1" )
ENDIF (PAHO_WITH_SSL)

# SHA1 test
ADD_EXECUTABLE( Sha1Test EXCLUDE_FROM_ALL SHA1.c SHA1.h )
TARGET_COMPILE_DEFINITIONS( Sha1Test PUBLIC "-DSHA1_TEST" )
IF (PAHO_WITH_SSL)
	ADD_EXECUTABLE( Sha1TestOpenSSL EXCLUDE_FROM_ALL SHA1.c SHA1.h )
	TARGET_LINK_LIBRARIES( Sha1TestOpenSSL ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} )
	TARGET_COMPILE_DEFINITIONS( Sha1TestOpenSSL PUBLIC "-DSHA1_TEST -DOPENSSL=1" )
ENDIF (PAHO_WITH_SSL)
