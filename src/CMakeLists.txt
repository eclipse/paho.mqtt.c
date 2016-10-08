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
configure_file(VersionInfo.h.in
    ${CMAKE_BINARY_DIR}/VersionInfo.h
    @ONLY
)

set(common_src
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

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(LIBS_SYSTEM ws2_32)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LIBS_SYSTEM dl)
endif()

add_executable(MQTTVersion MQTTVersion.c)
add_library(paho-mqtt3c SHARED ${common_src} MQTTClient.c)
add_library(paho-mqtt3a SHARED ${common_src} MQTTAsync.c)
target_include_directories(paho-mqtt3c PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_BINARY_DIR}
)
target_include_directories(paho-mqtt3a PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_BINARY_DIR}
)
target_link_libraries(paho-mqtt3c pthread ${LIBS_SYSTEM})
target_link_libraries(paho-mqtt3a pthread ${LIBS_SYSTEM})
target_link_libraries(MQTTVersion paho-mqtt3a paho-mqtt3c ${LIBS_SYSTEM})
set_target_properties(
    paho-mqtt3c paho-mqtt3a PROPERTIES
    VERSION ${CLIENT_VERSION}
    SOVERSION ${PAHO_VERSION_MAJOR}
    PREFIX "${PAHO_LIB_PREFIX}"
    IMPORT_PREFIX "${PAHO_LIB_PREFIX}"
)
install(TARGETS paho-mqtt3c paho-mqtt3a MQTTVersion
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(FILES MQTTAsync.h MQTTClient.h MQTTClientPersistence.h
    DESTINATION include
)

if (PAHO_WITH_SSL)
    find_package(OpenSSL REQUIRED)
    add_library(paho-mqtt3cs SHARED ${common_src} MQTTClient.c SSLSocket.c)
    add_library(paho-mqtt3as SHARED ${common_src} MQTTAsync.c SSLSocket.c)
    target_include_directories(paho-mqtt3cs PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_BINARY_DIR}
        ${OPENSSL_INCLUDE_DIR}
    )
    target_include_directories(paho-mqtt3as PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_BINARY_DIR}
        ${OPENSSL_INCLUDE_DIR}
    )
    target_link_libraries(paho-mqtt3cs pthread ${OPENSSL_LIBRARIES} ${LIBS_SYSTEM})
    target_link_libraries(paho-mqtt3as pthread ${OPENSSL_LIBRARIES} ${LIBS_SYSTEM})
    set_target_properties(
        paho-mqtt3cs paho-mqtt3as PROPERTIES
        VERSION ${CLIENT_VERSION}
        SOVERSION ${PAHO_VERSION_MAJOR}
        PREFIX "${PAHO_LIB_PREFIX}"
        IMPORT_PREFIX "${PAHO_LIB_PREFIX}"
    )
    install(TARGETS paho-mqtt3cs paho-mqtt3as
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

