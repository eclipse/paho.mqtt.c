# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

########################################
# Target Executable: paho_cs_pub
include $(CLEAR_VARS)

LOCAL_SRC_FILES := paho_cs_pub.c
LOCAL_MODULE := paho_cs_pub
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3c

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: paho_cs_sub
include $(CLEAR_VARS)

LOCAL_SRC_FILES := paho_cs_sub.c
LOCAL_MODULE := paho_cs_sub
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3c

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: MQTTClient_publish
include $(CLEAR_VARS)

LOCAL_SRC_FILES := MQTTClient_publish.c
LOCAL_MODULE := MQTTClient_publish
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3c

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: MQTTClient_publish_async
include $(CLEAR_VARS)

LOCAL_SRC_FILES := MQTTClient_publish_async.c
LOCAL_MODULE := MQTTClient_publish_async
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3c

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: MQTTClient_subscribe
include $(CLEAR_VARS)

LOCAL_SRC_FILES := MQTTClient_subscribe.c
LOCAL_MODULE := MQTTClient_subscribe
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3c

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: paho_c_pub
include $(CLEAR_VARS)

LOCAL_SRC_FILES := paho_c_pub.c
LOCAL_MODULE := paho_c_pub
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3a

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: paho_c_sub
include $(CLEAR_VARS)

LOCAL_SRC_FILES := paho_c_sub.c
LOCAL_MODULE := paho_c_sub
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3a

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: MQTTAsync_publish
include $(CLEAR_VARS)

LOCAL_SRC_FILES := MQTTAsync_publish.c
LOCAL_MODULE := MQTTAsync_publish
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3a

include $(BUILD_EXECUTABLE)

########################################
# Target Executable: MQTTAsync_subscribe
include $(CLEAR_VARS)

LOCAL_SRC_FILES := MQTTAsync_subscribe.c
LOCAL_MODULE := MQTTAsync_subscribe
LOCAL_MODULE_TAGS := test
LOCAL_C_INCLUDES := $(LOCAL_PATH)/..
LOCAL_SHARED_LIBRARIES := libpaho-mqtt3a

include $(BUILD_EXECUTABLE)
