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
bin_PROGRAMS += stdinpub
bin_PROGRAMS += stdoutsub
bin_PROGRAMS += pubsync
bin_PROGRAMS += pubasync
bin_PROGRAMS += subasync
endif
if PAHO_BUILD_ASYNC
bin_PROGRAMS += stdinpuba
bin_PROGRAMS += stdoutsuba
bin_PROGRAMS += MQTTAsync_publish
bin_PROGRAMS += MQTTAsync_subscribe
endif

###############################################################################
# Source code
###############################################################################

if PAHO_BUILD_SYNC
stdinpub_SOURCES = stdinpub.c

stdoutsub_SOURCES = stdoutsub.c

pubsync_SOURCES = pubsync.c

pubasync_SOURCES = pubasync.c

subasync_SOURCES = subasync.c
endif

if PAHO_BUILD_ASYNC
stdinpuba_SOURCES = stdinpuba.c

stdoutsuba_SOURCES = stdoutsuba.c

MQTTAsync_publish_SOURCES = MQTTAsync_publish.c

MQTTAsync_subscribe_SOURCES = MQTTAsync_subscribe.c
endif

###############################################################################
# Compiler flags
###############################################################################

COMMONCFLAGS  = $(CFLAGS)
COMMONCFLAGS += -I$(top_srcdir)/src/

if PAHO_BUILD_SYNC
stdinpub_CFLAGS = ${COMMONCFLAGS}

stdoutsub_CFLAGS = ${COMMONCFLAGS}

pubsync_CFLAGS = ${COMMONCFLAGS}

pubasync_CFLAGS = ${COMMONCFLAGS}

subasync_CFLAGS = ${COMMONCFLAGS}
endif

if PAHO_BUILD_ASYNC
stdinpuba_CFLAGS = ${COMMONCFLAGS}

stdoutsuba_CFLAGS = ${COMMONCFLAGS}

MQTTAsync_publish_CFLAGS = ${COMMONCFLAGS}

MQTTAsync_subscribe_CFLAGS = ${COMMONCFLAGS}
endif

###############################################################################
# Linker flags
###############################################################################

if PAHO_BUILD_SYNC
stdinpub_LDFLAGS = $(LDFLAGS)
stdinpub_LDADD  = $(LIBS)
stdinpub_LDADD += ../libpaho-mqtt3c.la

stdoutsub_LDFLAGS = $(LDFLAGS)
stdoutsub_LDADD  = $(LIBS)
stdoutsub_LDADD += ../libpaho-mqtt3c.la

pubsync_LDFLAGS = $(LDFLAGS)
pubsync_LDADD  = $(LIBS)
pubsync_LDADD += ../libpaho-mqtt3c.la

pubasync_LDFLAGS = $(LDFLAGS)
pubasync_LDADD  = $(LIBS)
pubasync_LDADD += ../libpaho-mqtt3c.la

subasync_LDFLAGS = $(LDFLAGS)
subasync_LDADD  = $(LIBS)
subasync_LDADD += ../libpaho-mqtt3c.la
endif

if PAHO_BUILD_ASYNC
stdinpuba_LDFLAGS = $(LDFLAGS)
stdinpuba_LDADD  = $(LIBS)
stdinpuba_LDADD += ../libpaho-mqtt3a.la

stdoutsuba_LDFLAGS = $(LDFLAGS)
stdoutsuba_LDADD  = $(LIBS)
stdoutsuba_LDADD += ../libpaho-mqtt3a.la

MQTTAsync_publish_LDFLAGS = $(LDFLAGS)
MQTTAsync_publish_LDADD  = $(LIBS)
MQTTAsync_publish_LDADD += ../libpaho-mqtt3a.la

MQTTAsync_subscribe_LDFLAGS = $(LDFLAGS)
MQTTAsync_subscribe_LDADD  = $(LIBS)
MQTTAsync_subscribe_LDADD += ../libpaho-mqtt3a.la
endif
