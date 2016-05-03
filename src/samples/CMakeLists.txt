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
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
    )

# sample files c
ADD_EXECUTABLE(stdinpub stdinpub.c)
ADD_EXECUTABLE(stdoutsub stdoutsub.c)
ADD_EXECUTABLE(pubsync pubsync.c)
ADD_EXECUTABLE(pubasync pubasync.c)
ADD_EXECUTABLE(subasync subasync.c)

TARGET_LINK_LIBRARIES(stdinpub paho-mqtt3c)
TARGET_LINK_LIBRARIES(stdoutsub paho-mqtt3c)
TARGET_LINK_LIBRARIES(pubsync paho-mqtt3c)
TARGET_LINK_LIBRARIES(pubasync paho-mqtt3c)
TARGET_LINK_LIBRARIES(subasync paho-mqtt3c)

ADD_EXECUTABLE(stdinpuba stdinpuba.c)
ADD_EXECUTABLE(stdoutsuba stdoutsuba.c)
ADD_EXECUTABLE(MQTTAsync_subscribe MQTTAsync_subscribe.c)
ADD_EXECUTABLE(MQTTAsync_publish MQTTAsync_publish.c)

TARGET_LINK_LIBRARIES(stdinpuba paho-mqtt3a)
TARGET_LINK_LIBRARIES(stdoutsuba paho-mqtt3a)
TARGET_LINK_LIBRARIES(MQTTAsync_subscribe paho-mqtt3a)
TARGET_LINK_LIBRARIES(MQTTAsync_publish paho-mqtt3a)

INSTALL(TARGETS stdinpub
                stdoutsub
                pubsync
                pubasync
                subasync
                stdinpuba
                stdoutsuba
                MQTTAsync_subscribe
                MQTTAsync_publish
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

