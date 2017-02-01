#*******************************************************************************
#  Copyright (c) 2015 logi.cals GmbH
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
    )

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET(LIBS_SYSTEM ws2_32)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET(LIBS_SYSTEM dl)
ENDIF()

IF (UNIX)
    SET(THREAD_LIBS_SYSTEM pthread)
ENDIF()

ADD_EXECUTABLE(MQTTVersion MQTTVersion.c)
ADD_LIBRARY(paho-mqtt3c SHARED ${common_src} MQTTClient.c)
ADD_LIBRARY(paho-mqtt3a SHARED ${common_src} MQTTAsync.c)
TARGET_LINK_LIBRARIES(paho-mqtt3c ${THREAD_LIBS_SYSTEM} ${LIBS_SYSTEM})
TARGET_LINK_LIBRARIES(paho-mqtt3a ${THREAD_LIBS_SYSTEM} ${LIBS_SYSTEM})
TARGET_LINK_LIBRARIES(MQTTVersion paho-mqtt3a paho-mqtt3c ${LIBS_SYSTEM})
SET_TARGET_PROPERTIES(
    paho-mqtt3c paho-mqtt3a PROPERTIES
    VERSION ${CLIENT_VERSION}
    SOVERSION ${PAHO_VERSION_MAJOR})
INSTALL(TARGETS paho-mqtt3c paho-mqtt3a MQTTVersion
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${PAHO_LIBRARY_DIR_NAME})
INSTALL(FILES MQTTAsync.h MQTTClient.h MQTTClientPersistence.h
    DESTINATION include)

IF (PAHO_WITH_SSL)
SET(OPENSSL_LIB_SEARCH_PATH "" CACHE PATH "Directory containing OpenSSL libraries")
    SET(OPENSSL_INC_SEARCH_PATH "" CACHE PATH "Directory containing OpenSSL includes")

    IF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
      SET(OPENSSL_BREW_PATH "/usr/local/opt/openssl")
    ENDIF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

    FIND_PATH(OPENSSL_INCLUDE_DIR openssl/ssl.h
        HINTS ${OPENSSL_INC_SEARCH_PATH}/include ${OPENSSL_BREW_PATH}/include/)
    FIND_LIBRARY(OPENSSL_LIB NAMES ssl libssl
        HINTS ${OPENSSL_BREW_PATH}/lib ${OPENSSL_DIR}/lib ${OPENSSL_DIR}/lib64)
    FIND_LIBRARY(OPENSSLCRYPTO_LIB NAMES ssl libssl
      	HINTS ${OPENSSL_BREW_PATH}/lib ${OPENSSL_DIR}/lib ${OPENSSL_DIR}/lib64)

    MESSAGE(STATUS "OpenSSL hint ${PENSSL_INC_SEARCH_PATH} (includes) / ")
    MESSAGE(STATUS "OpenSSL headers found at ${OPENSSL_INCLUDE_DIR}")
    MESSAGE(STATUS "OpenSSL library found at ${OPENSSL_LIB}")
    MESSAGE(STATUS "OpenSSL Crypto library found at ${OPENSSLCRYPTO_LIB}")

    INCLUDE_DIRECTORIES(
        ${OPENSSL_INCLUDE_DIR}
    )
    ADD_LIBRARY(paho-mqtt3cs SHARED ${common_src} MQTTClient.c SSLSocket.c)
    ADD_LIBRARY(paho-mqtt3as SHARED ${common_src} MQTTAsync.c SSLSocket.c)

    TARGET_LINK_LIBRARIES(paho-mqtt3cs ${THREAD_LIBS_SYSTEM} ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} ${LIBS_SYSTEM})
    TARGET_LINK_LIBRARIES(paho-mqtt3as ${THREAD_LIBS_SYSTEM} ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} ${LIBS_SYSTEM})
    SET_TARGET_PROPERTIES(
        paho-mqtt3cs paho-mqtt3as PROPERTIES
        VERSION ${CLIENT_VERSION}
        SOVERSION ${PAHO_VERSION_MAJOR})
    INSTALL(TARGETS paho-mqtt3cs
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PAHO_LIBRARY_DIR_NAME})
    INSTALL(TARGETS paho-mqtt3as
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PAHO_LIBRARY_DIR_NAME})
ENDIF()
