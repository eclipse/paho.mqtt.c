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
# Build-time header
###############################################################################

TIMESTAMP = `date`

SED_COMMAND = $(SED) \
    -e "s/@CLIENT_VERSION@/$(PACKAGE_VERSION)/g" \
    -e "s/@BUILD_TIMESTAMP@/${TIMESTAMP}/g"

VERSION_INFO_TERMPLATE = VersionInfo.h.in

# For BSD builds, using a variable instead of $<
VersionInfo.h: $(VERSION_INFO_TERMPLATE)
	$(SED_COMMAND) $(VERSION_INFO_TERMPLATE) > $@

BUILT_SOURCES = VersionInfo.h

###############################################################################
# Public API headers (to install)
###############################################################################

include_HEADERS = MQTTClientPersistence.h

if PAHO_BUILD_SYNC
include_HEADERS += MQTTClient.h
endif

if PAHO_BUILD_ASYNC
include_HEADERS += MQTTAsync.h
endif

###############################################################################
# Distribuition headers (to compile distribuition)
###############################################################################

EXTRA_DIST  = $(VERSION_INFO_TERMPLATE)
EXTRA_DIST += Clients.h
EXTRA_DIST += Heap.h
EXTRA_DIST += LinkedList.h
EXTRA_DIST += Log.h
EXTRA_DIST += Messages.h
EXTRA_DIST += MQTTPacket.h
EXTRA_DIST += MQTTPacketOut.h
EXTRA_DIST += MQTTPersistenceDefault.h
EXTRA_DIST += MQTTPersistence.h
EXTRA_DIST += MQTTProtocolClient.h
EXTRA_DIST += MQTTProtocol.h
EXTRA_DIST += MQTTProtocolOut.h
EXTRA_DIST += SocketBuffer.h
EXTRA_DIST += Socket.h
EXTRA_DIST += SSLSocket.h
EXTRA_DIST += StackTrace.h
EXTRA_DIST += Thread.h
EXTRA_DIST += Tree.h
EXTRA_DIST += utf-8.h

###############################################################################
# Libraries names
###############################################################################

# Convenience Library to avoid compile twice
noinst_LTLIBRARIES  = libcommon.la

lib_LTLIBRARIES  =

if PAHO_BUILD_SYNC
lib_LTLIBRARIES += libpaho-mqtt3c.la
endif

if PAHO_BUILD_ASYNC
lib_LTLIBRARIES += libpaho-mqtt3a.la
endif

if PAHO_WITH_SSL

# Convenience Library to avoid compile twice
noinst_LTLIBRARIES += libcommon_ssl.la

if PAHO_BUILD_SYNC
lib_LTLIBRARIES += libpaho-mqtt3cs.la
endif

if PAHO_BUILD_ASYNC
lib_LTLIBRARIES += libpaho-mqtt3as.la
endif

endif

###############################################################################
# Libraries source code
###############################################################################

COMMONSOURCES  = Clients.c
COMMONSOURCES += Heap.c
COMMONSOURCES += LinkedList.c
COMMONSOURCES += Log.c
COMMONSOURCES += Messages.c
COMMONSOURCES += MQTTPacket.c
COMMONSOURCES += MQTTPacketOut.c
COMMONSOURCES += MQTTPersistence.c
COMMONSOURCES += MQTTPersistenceDefault.c
COMMONSOURCES += MQTTProtocolClient.c
COMMONSOURCES += MQTTProtocolOut.c
COMMONSOURCES += SocketBuffer.c
COMMONSOURCES += Socket.c
COMMONSOURCES += StackTrace.c
COMMONSOURCES += Thread.c
COMMONSOURCES += Tree.c
COMMONSOURCES += utf-8.c

libcommon_la_SOURCES  = ${COMMONSOURCES}

if PAHO_BUILD_SYNC
libpaho_mqtt3c_la_SOURCES  = MQTTClient.c
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3a_la_SOURCES  = MQTTAsync.c
endif

if PAHO_WITH_SSL

libcommon_ssl_la_SOURCES  = ${COMMONSOURCES}

if PAHO_BUILD_SYNC
libpaho_mqtt3cs_la_SOURCES = MQTTClient.c SSLSocket.c
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3as_la_SOURCES = MQTTAsync.c SSLSocket.c
endif

endif

###############################################################################
# Libraries compiler flags
###############################################################################

COMMONCFLAGS  = $(CFLAGS)
COMMONCFLAGS += -fPIC

libcommon_la_CFLAGS  = ${COMMONCFLAGS}

if PAHO_BUILD_SYNC
libpaho_mqtt3c_la_CFLAGS  = ${COMMONCFLAGS}
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3a_la_CFLAGS  = ${COMMONCFLAGS}
endif

if PAHO_WITH_SSL

libcommon_ssl_la_CFLAGS  = ${COMMONCFLAGS} -DOPENSSL

if PAHO_BUILD_SYNC
libpaho_mqtt3cs_la_CFLAGS = ${COMMONCFLAGS} -DOPENSSL
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3as_la_CFLAGS = ${COMMONCFLAGS} -DOPENSSL
endif

endif

###############################################################################
# Libraries linker flags
###############################################################################

# Create SONAME entry. Same as -Wl,-soname,libname.so.major
VERSION_INFO = `echo '$(PACKAGE_VERSION)' | $(SED) -e "s/\(.*\)\.\(.*\\)\.\(.*\)/\1:\3:\2/g" `
COMMONLDFLAGS  = -version-info $(VERSION_INFO)

COMMONLDFLAGS += $(LDFLAGS)
COMMONLDFLAGS += $(LIBS)

SYNCLDFLAGS   = -Wl,-init,MQTTClient_init

ASYNCLDFLAGS  = -Wl,-init,MQTTAsync_init

if PAHO_BUILD_SYNC
libpaho_mqtt3c_la_LDFLAGS  = ${COMMONLDFLAGS}
libpaho_mqtt3c_la_LDFLAGS += ${SYNCLDFLAGS}

libpaho_mqtt3c_la_LIBADD  = libcommon.la
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3a_la_LDFLAGS  = ${COMMONLDFLAGS}
libpaho_mqtt3a_la_LDFLAGS += ${ASYNCLDFLAGS}

libpaho_mqtt3a_la_LIBADD  = libcommon.la
endif

if PAHO_WITH_SSL

SSLLDFLAGS  = -Wl,-no-whole-archive

if PAHO_BUILD_SYNC
libpaho_mqtt3cs_la_LDFLAGS  = ${COMMONLDFLAGS}
libpaho_mqtt3cs_la_LDFLAGS += ${SYNCLDFLAGS}
libpaho_mqtt3cs_la_LDFLAGS += ${SSLLDFLAGS}

libpaho_mqtt3cs_la_LIBADD  = libcommon_ssl.la
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3as_la_LDFLAGS  = ${COMMONLDFLAGS}
libpaho_mqtt3as_la_LDFLAGS += ${ASYNCLDFLAGS}
libpaho_mqtt3as_la_LDFLAGS += ${SSLLDFLAGS}

libpaho_mqtt3as_la_LIBADD  = libcommon_ssl.la
endif

endif

###############################################################################
# Binary name
###############################################################################

bin_PROGRAMS = MQTTVersion

###############################################################################
# Binary source code
###############################################################################

MQTTVersion_SOURCES = MQTTVersion.c

###############################################################################
# Binary compiler flags
###############################################################################

MQTTVersion_CFLAGS = $(CFLAGS)

###############################################################################
# Binary linker flags
###############################################################################

MQTTVersion_LDFLAGS = $(LDFLAGS)

MQTTVersion_LDADD  = $(LIBS)
MQTTVersion_LDADD += libpaho-mqtt3a.la
