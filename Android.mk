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

common_src_files := \
    src/Heap.c \
    src/LinkedList.c \
    src/MQTTProtocolClient.c \
    src/MQTTProtocolOut.c \
    src/MQTTPersistenceDefault.c \
    src/Messages.c \
    src/MQTTPacketOut.c \
    src/Clients.c \
    src/Thread.c \
    src/MQTTPacket.c \
    src/MQTTPersistence.c \
    src/Log.c \
    src/StackTrace.c \
    src/utf-8.c \
    src/Socket.c \
    src/SocketBuffer.c \
    src/Tree.c

########################################
# Target share libraries: libpaho-mqtt3c
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(common_src_files) \
    src/MQTTClient.c
LOCAL_LDFLAGS := -Wl,-init,MQTTClient_init
LOCAL_MODULE := libpaho-mqtt3c
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := paho
include $(LOCAL_PATH)/VersionInfo.mk

include $(BUILD_SHARED_LIBRARY)

########################################
# Target share libraries: libpaho-mqtt3cs
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(common_src_files) \
    src/MQTTClient.c \
    src/SSLSocket.c
LOCAL_CFLAGS := -DOPENSSL -DBORINGSSL
LOCAL_LDFLAGS := -Wl,-init,MQTTClient_init
LOCAL_MODULE := libpaho-mqtt3cs
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := paho
LOCAL_SHARED_LIBRARIES := libcrypto libssl
LOCAL_C_INCLUDE := external/boringssl/src/include
include $(LOCAL_PATH)/VersionInfo.mk

include $(BUILD_SHARED_LIBRARY)

########################################
# Target share libraries: libpaho-mqtt3a
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(common_src_files) \
    src/MQTTAsync.c
LOCAL_LDFLAGS := -Wl,-init,MQTTAsync_init
LOCAL_MODULE := libpaho-mqtt3a
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := paho
include $(LOCAL_PATH)/VersionInfo.mk

include $(BUILD_SHARED_LIBRARY)

########################################
# Target share libraries: libpaho-mqtt3as
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(common_src_files) \
    src/MQTTAsync.c \
    src/SSLSocket.c
LOCAL_CFLAGS := -DOPENSSL -DBORINGSSL
LOCAL_LDFLAGS := -Wl,-init,MQTTAsync_init
LOCAL_MODULE := libpaho-mqtt3as
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := paho
LOCAL_SHARED_LIBRARIES := libcrypto libssl
LOCAL_C_INCLUDE := external/boringssl/src/include
include $(LOCAL_PATH)/VersionInfo.mk

include $(BUILD_SHARED_LIBRARY)


########################################
include $(LOCAL_PATH)/src/samples/Android.mk
