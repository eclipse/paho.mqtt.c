#*******************************************************************************
#  Copyright (c) 2015, 2016 logi.cals GmbH
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
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
    )

# sample files c
ADD_EXECUTABLE(paho_c_pub paho_c_pub.c)
ADD_EXECUTABLE(paho_c_sub paho_c_sub.c)
ADD_EXECUTABLE(paho_cs_pub paho_cs_pub.c)
ADD_EXECUTABLE(paho_cs_sub paho_cs_sub.c)

TARGET_LINK_LIBRARIES(paho_c_pub paho-mqtt3a)
TARGET_LINK_LIBRARIES(paho_c_sub paho-mqtt3a)
TARGET_LINK_LIBRARIES(paho_cs_pub paho-mqtt3c)
TARGET_LINK_LIBRARIES(paho_cs_sub paho-mqtt3c)

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
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

