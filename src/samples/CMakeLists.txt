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

# sample files c
add_executable(paho_c_pub paho_c_pub.c)
add_executable(paho_c_sub paho_c_sub.c)
add_executable(paho_cs_pub paho_cs_pub.c)
add_executable(paho_cs_sub paho_cs_sub.c)

target_include_directories(paho_c_pub PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(paho_c_sub PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(paho_cs_pub PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(paho_cs_sub PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(paho_c_pub paho-mqtt3a)
target_link_libraries(paho_c_sub paho-mqtt3a)
target_link_libraries(paho_cs_pub paho-mqtt3c)
target_link_libraries(paho_cs_sub paho-mqtt3c)

add_executable(MQTTAsync_subscribe MQTTAsync_subscribe.c)
add_executable(MQTTAsync_publish MQTTAsync_publish.c)
add_executable(MQTTClient_subscribe MQTTClient_subscribe.c)
add_executable(MQTTClient_publish MQTTClient_publish.c)
add_executable(MQTTClient_publish_async MQTTClient_publish_async.c)

target_include_directories(MQTTAsync_subscribe PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(MQTTAsync_publish PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(MQTTClient_subscribe PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(MQTTClient_publish PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(MQTTClient_publish_async PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(MQTTAsync_subscribe paho-mqtt3a)
target_link_libraries(MQTTAsync_publish paho-mqtt3a)
target_link_libraries(MQTTClient_subscribe paho-mqtt3c)
target_link_libraries(MQTTClient_publish paho-mqtt3c)
target_link_libraries(MQTTClient_publish_async paho-mqtt3c)

install(TARGETS paho_c_sub
                paho_c_pub
                paho_cs_sub
                paho_cs_pub
                MQTTAsync_subscribe
                MQTTAsync_publish
                MQTTClient_subscribe
                MQTTClient_publish
                MQTTClient_publish_async
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

