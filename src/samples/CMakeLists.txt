#*******************************************************************************
#  Copyright (c) 2015, 2017 logi.cals GmbH and others
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
#     Ian Craggs - update sample names
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

## compilation/linkage settings
INCLUDE_DIRECTORIES(
    .
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
    )

IF (WIN32)
  	ADD_DEFINITIONS(/DCMAKE_BUILD /D_CRT_SECURE_NO_DEPRECATE)
ENDIF()

# sample files c
ADD_EXECUTABLE(paho_c_pub paho_c_pub.c pubsub_opts.c)
ADD_EXECUTABLE(paho_c_sub paho_c_sub.c pubsub_opts.c)
ADD_EXECUTABLE(paho_cs_pub paho_cs_pub.c pubsub_opts.c)
ADD_EXECUTABLE(paho_cs_sub paho_cs_sub.c pubsub_opts.c)

TARGET_LINK_LIBRARIES(paho_c_pub paho-mqtt3as)
TARGET_LINK_LIBRARIES(paho_c_sub paho-mqtt3as)
TARGET_LINK_LIBRARIES(paho_cs_pub paho-mqtt3cs)
TARGET_LINK_LIBRARIES(paho_cs_sub paho-mqtt3cs)

ADD_EXECUTABLE(MQTTAsync_subscribe MQTTAsync_subscribe.c)
ADD_EXECUTABLE(MQTTAsync_publish MQTTAsync_publish.c)
ADD_EXECUTABLE(MQTTClient_subscribe MQTTClient_subscribe.c)
ADD_EXECUTABLE(MQTTClient_publish MQTTClient_publish.c)
ADD_EXECUTABLE(MQTTClient_publish_async MQTTClient_publish_async.c)

TARGET_LINK_LIBRARIES(MQTTAsync_subscribe paho-mqtt3a)
TARGET_LINK_LIBRARIES(MQTTAsync_publish paho-mqtt3a)
TARGET_LINK_LIBRARIES(MQTTClient_subscribe paho-mqtt3c)
TARGET_LINK_LIBRARIES(MQTTClient_publish paho-mqtt3c)
TARGET_LINK_LIBRARIES(MQTTClient_publish_async paho-mqtt3c)

INSTALL(TARGETS paho_c_sub
                paho_c_pub
                paho_cs_sub
                paho_cs_pub
                MQTTAsync_subscribe
                MQTTAsync_publish
                MQTTClient_subscribe
                MQTTClient_publish
                MQTTClient_publish_async

    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
