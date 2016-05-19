#*******************************************************************************
#  Copyright (c) 2016
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
#     Guilherme Maciel Ferreira - initial implementation
#*******************************************************************************/

###############################################################################
# Binaries names
###############################################################################

bin_PROGRAMS  =

if PAHO_BUILD_SYNC
bin_PROGRAMS += paho_cs_pub
bin_PROGRAMS += paho_cs_sub
bin_PROGRAMS += MQTTClient_publish_async
bin_PROGRAMS += MQTTClient_publish
bin_PROGRAMS += MQTTClient_subscribe
endif

if PAHO_BUILD_ASYNC
bin_PROGRAMS += paho_c_pub
bin_PROGRAMS += paho_c_sub
bin_PROGRAMS += MQTTAsync_publish
bin_PROGRAMS += MQTTAsync_subscribe
endif

###############################################################################
# Source code
###############################################################################

if PAHO_BUILD_SYNC
paho_cs_pub_SOURCES = paho_cs_pub.c

paho_cs_sub_SOURCES = paho_cs_sub.c

MQTTClient_publish_async_SOURCES = MQTTClient_publish_async.c

MQTTClient_publish_SOURCES = MQTTClient_publish.c

MQTTClient_subscribe_SOURCES = MQTTClient_subscribe.c
endif

if PAHO_BUILD_ASYNC
paho_c_pub_SOURCES = paho_c_pub.c

paho_c_sub_SOURCES = paho_c_sub.c

MQTTAsync_publish_SOURCES = MQTTAsync_publish.c

MQTTAsync_subscribe_SOURCES = MQTTAsync_subscribe.c
endif

###############################################################################
# Compiler flags
###############################################################################

COMMONCFLAGS  = $(CFLAGS)
COMMONCFLAGS += -I$(top_srcdir)/src/

if PAHO_BUILD_SYNC
paho_cs_pub_CFLAGS = ${COMMONCFLAGS}

paho_cs_sub_CFLAGS = ${COMMONCFLAGS}

MQTTClient_publish_async_CFLAGS = ${COMMONCFLAGS}

MQTTClient_publish_CFLAGS = ${COMMONCFLAGS}

MQTTClient_subscribe_CFLAGS = ${COMMONCFLAGS}
endif

if PAHO_BUILD_ASYNC
paho_c_pub_CFLAGS = ${COMMONCFLAGS}

paho_c_sub_CFLAGS = ${COMMONCFLAGS}

MQTTAsync_publish_CFLAGS = ${COMMONCFLAGS}

MQTTAsync_subscribe_CFLAGS = ${COMMONCFLAGS}
endif

###############################################################################
# Linker flags
###############################################################################

if PAHO_BUILD_SYNC
paho_cs_pub_LDFLAGS = $(LDFLAGS)
paho_cs_pub_LDADD  = $(LIBS)
paho_cs_pub_LDADD += $(top_srcdir)/src/libpaho-mqtt3c.la

paho_cs_sub_LDFLAGS = $(LDFLAGS)
paho_cs_sub_LDADD  = $(LIBS)
paho_cs_sub_LDADD += $(top_srcdir)/src/libpaho-mqtt3c.la

MQTTClient_publish_async_LDFLAGS = $(LDFLAGS)
MQTTClient_publish_async_LDADD  = $(LIBS)
MQTTClient_publish_async_LDADD += $(top_srcdir)/src/libpaho-mqtt3c.la

MQTTClient_publish_LDFLAGS = $(LDFLAGS)
MQTTClient_publish_LDADD  = $(LIBS)
MQTTClient_publish_LDADD += $(top_srcdir)/src/libpaho-mqtt3c.la

MQTTClient_subscribe_LDFLAGS = $(LDFLAGS)
MQTTClient_subscribe_LDADD  = $(LIBS)
MQTTClient_subscribe_LDADD += $(top_srcdir)/src/libpaho-mqtt3c.la
endif

if PAHO_BUILD_ASYNC
paho_c_pub_LDFLAGS = $(LDFLAGS)
paho_c_pub_LDADD  = $(LIBS)
paho_c_pub_LDADD += $(top_srcdir)/src/libpaho-mqtt3a.la

paho_c_sub_LDFLAGS = $(LDFLAGS)
paho_c_sub_LDADD  = $(LIBS)
paho_c_sub_LDADD += $(top_srcdir)/src/libpaho-mqtt3a.la

MQTTAsync_publish_LDFLAGS = $(LDFLAGS)
MQTTAsync_publish_LDADD  = $(LIBS)
MQTTAsync_publish_LDADD += $(top_srcdir)/src/libpaho-mqtt3a.la

MQTTAsync_subscribe_LDFLAGS = $(LDFLAGS)
MQTTAsync_subscribe_LDADD  = $(LIBS)
MQTTAsync_subscribe_LDADD += $(top_srcdir)/src/libpaho-mqtt3a.la
endif
