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

AUTOMAKE_OPTIONS = subdir-objects color-tests
ACLOCAL_AMFLAGS = -I m4

###############################################################################
# Common precompiler, compiler and linker flags
###############################################################################

# AM_CPPFLAGS are added by default to the precompiler flags CPPFLAGS within automake, except there are more special CPPFLAGS
AM_CPPFLAGS = -I $(srcdir)/src -I src
# AM_CFLAGS are added by default to the CFLAGS within automake, except there are more special CFLAGS
AM_CFLAGS =

# Libraries with openssl support need to be build with macro OPENSSL defined.
libcommon_ssl_la_CPPFLAGS  = $(AM_CPPFLAGS) -DOPENSSL
libpaho_mqtt3cs_la_CPPFLAGS = $(AM_CPPFLAGS) -DOPENSSL
libpaho_mqtt3as_la_CFLAGS = $(AM_CPPFLAGS) -DOPENSSL

###############################################################################
# Build-time header
###############################################################################

TIMESTAMP = `date`

SED_COMMAND = $(SED) \
    -e "s/@CLIENT_VERSION@/$(PACKAGE_VERSION)/g" \
    -e "s/@BUILD_TIMESTAMP@/${TIMESTAMP}/g"

VERSION_INFO_TERMPLATE = $(srcdir)/src/VersionInfo.h.in

# For BSD builds, using a variable instead of $<
src/VersionInfo.h: $(VERSION_INFO_TERMPLATE)
	$(SED_COMMAND) $(VERSION_INFO_TERMPLATE) > $@

BUILT_SOURCES = src/VersionInfo.h

###############################################################################
# Install: public API headers
###############################################################################

include_HEADERS = src/MQTTClientPersistence.h

if PAHO_BUILD_SYNC
include_HEADERS += src/MQTTClient.h
endif

if PAHO_BUILD_ASYNC
include_HEADERS += src/MQTTAsync.h
endif

###############################################################################
# Distribuition: headers to compile distribuition
###############################################################################

noinst_HEADERS  = $(VERSION_INFO_TERMPLATE)
noinst_HEADERS += src/Clients.h
noinst_HEADERS += src/Heap.h
noinst_HEADERS += src/LinkedList.h
noinst_HEADERS += src/Log.h
noinst_HEADERS += src/Messages.h
noinst_HEADERS += src/MQTTPacket.h
noinst_HEADERS += src/MQTTPacketOut.h
noinst_HEADERS += src/MQTTPersistenceDefault.h
noinst_HEADERS += src/MQTTPersistence.h
noinst_HEADERS += src/MQTTProtocolClient.h
noinst_HEADERS += src/MQTTProtocol.h
noinst_HEADERS += src/MQTTProtocolOut.h
noinst_HEADERS += src/SocketBuffer.h
noinst_HEADERS += src/Socket.h
noinst_HEADERS += src/SSLSocket.h
noinst_HEADERS += src/StackTrace.h
noinst_HEADERS += src/Thread.h
noinst_HEADERS += src/Tree.h
noinst_HEADERS += src/utf-8.h

###############################################################################
# Libraries: names
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
# Libraries: source code
###############################################################################

COMMONSOURCES  = src/Clients.c
COMMONSOURCES += src/Heap.c
COMMONSOURCES += src/LinkedList.c
COMMONSOURCES += src/Log.c
COMMONSOURCES += src/Messages.c
COMMONSOURCES += src/MQTTPacket.c
COMMONSOURCES += src/MQTTPacketOut.c
COMMONSOURCES += src/MQTTPersistence.c
COMMONSOURCES += src/MQTTPersistenceDefault.c
COMMONSOURCES += src/MQTTProtocolClient.c
COMMONSOURCES += src/MQTTProtocolOut.c
COMMONSOURCES += src/SocketBuffer.c
COMMONSOURCES += src/Socket.c
COMMONSOURCES += src/StackTrace.c
COMMONSOURCES += src/Thread.c
COMMONSOURCES += src/Tree.c
COMMONSOURCES += src/utf-8.c

libcommon_la_SOURCES  = ${COMMONSOURCES}

if PAHO_BUILD_SYNC
libpaho_mqtt3c_la_SOURCES  = src/MQTTClient.c
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3a_la_SOURCES  = src/MQTTAsync.c
endif

if PAHO_WITH_SSL

libcommon_ssl_la_SOURCES  = ${COMMONSOURCES}

if PAHO_BUILD_SYNC
libpaho_mqtt3cs_la_SOURCES = src/MQTTClient.c src/SSLSocket.c
endif

if PAHO_BUILD_ASYNC
libpaho_mqtt3as_la_SOURCES = src/MQTTAsync.c src/SSLSocket.c
endif

endif # if PAHO_WITH_SSL

###############################################################################
# Libraries: compiler flags
###############################################################################

#COMMONCFLAGS += -fPIC # do not set this here, because the libtool needs to control it!

###############################################################################
# Libraries: linker flags
###############################################################################

# Create SONAME entry. Same as -Wl,-soname,libname.so.major
VERSION_INFO = `echo '$(PACKAGE_VERSION)' | $(SED) -e "s/\(.*\)\.\(.*\\)\.\(.*\)/\1:\3:\2/g" `
COMMONLDFLAGS  = -version-info $(VERSION_INFO)

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

endif # if PAHO_WITH_SSL

###############################################################################
# Binary: name
###############################################################################

bin_PROGRAMS = MQTTVersion

###############################################################################
# Binary: source code
###############################################################################

MQTTVersion_SOURCES = src/MQTTVersion.c


###############################################################################
# Binary: linker flags
###############################################################################

MQTTVersion_LDADD = libpaho-mqtt3a.la


###############################################################################
# Samples: names
###############################################################################

if PAHO_BUILD_SAMPLES
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
endif # if PAHO_BUILD_SAMPLES

###############################################################################
# Samples: source code
###############################################################################

if PAHO_BUILD_SYNC
paho_cs_pub_SOURCES = src/samples/paho_cs_pub.c

paho_cs_sub_SOURCES = src/samples/paho_cs_sub.c

MQTTClient_publish_async_SOURCES = src/samples/MQTTClient_publish_async.c

MQTTClient_publish_SOURCES = src/samples/MQTTClient_publish.c

MQTTClient_subscribe_SOURCES = src/samples/MQTTClient_subscribe.c
endif

if PAHO_BUILD_ASYNC
paho_c_pub_SOURCES = src/samples/paho_c_pub.c

paho_c_sub_SOURCES = src/samples/paho_c_sub.c

MQTTAsync_publish_SOURCES = src/samples/MQTTAsync_publish.c

MQTTAsync_subscribe_SOURCES = src/samples/MQTTAsync_subscribe.c
endif

###############################################################################
# Samples: linker flags
###############################################################################

if PAHO_BUILD_SYNC
paho_cs_pub_LDADD = libpaho-mqtt3c.la

paho_cs_sub_LDADD = libpaho-mqtt3c.la

MQTTClient_publish_async_LDADD = libpaho-mqtt3c.la

MQTTClient_publish_LDADD = libpaho-mqtt3c.la

MQTTClient_subscribe_LDADD = libpaho-mqtt3c.la
endif

if PAHO_BUILD_ASYNC
paho_c_pub_LDADD = libpaho-mqtt3a.la

paho_c_sub_LDADD = libpaho-mqtt3a.la

MQTTAsync_publish_LDADD = libpaho-mqtt3a.la

MQTTAsync_subscribe_LDADD = libpaho-mqtt3a.la
endif

check_PROGRAMS  =

if PAHO_BUILD_SYNC
check_PROGRAMS += test1
check_PROGRAMS += test2
check_PROGRAMS += test_mqtt4sync
check_PROGRAMS += sync_client_test
endif

if PAHO_BUILD_ASYNC
check_PROGRAMS += test4
check_PROGRAMS += test6
check_PROGRAMS += test8
check_PROGRAMS += test9
check_PROGRAMS += test_mqtt4async
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
check_PROGRAMS += test3
endif

if PAHO_BUILD_ASYNC
check_PROGRAMS += test5
endif

endif

TESTS = $(check_PROGRAMS)

###############################################################################
# Test: source code
###############################################################################

if PAHO_BUILD_SYNC
test1_SOURCES = test/test1.c

test2_SOURCES = test/test2.c

test_mqtt4sync_SOURCES = test/test_mqtt4sync.c

sync_client_test_SOURCES = test/sync_client_test.c
endif

if PAHO_BUILD_ASYNC
test4_SOURCES = test/test4.c

test6_SOURCES = test/test6.c

test8_SOURCES = test/test8.c

test9_SOURCES = test/test9.c

test_mqtt4async_SOURCES = test/test_mqtt4async.c
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
test3_SOURCES = test/test3.c
endif

if PAHO_BUILD_ASYNC
test5_SOURCES = test/test5.c
endif

endif

###############################################################################
# Test: linker flags
###############################################################################

if PAHO_BUILD_SYNC

test1_LDADD = libpaho-mqtt3c.la

test2_LDADD = libpaho-mqtt3c.la

test_mqtt4sync_LDADD = libpaho-mqtt3c.la

sync_client_test_LDADD = libpaho-mqtt3c.la
endif

if PAHO_BUILD_ASYNC

test4_LDADD = libpaho-mqtt3a.la

test6_LDADD = libpaho-mqtt3a.la

test8_LDADD = libpaho-mqtt3a.la

test9_LDADD = libpaho-mqtt3a.la

test_mqtt4async_LDADD = libpaho-mqtt3a.la
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
test3_LDADD = libpaho-mqtt3cs.la
endif

if PAHO_BUILD_ASYNC
test5_LDADD = libpaho-mqtt3as.la
endif

endif

###############################################################################
# Documentation
###############################################################################

if PAHO_BUILD_DOC
DoxyfileV3ClientAPI.stamp:
	$(MKDIR_P) @top_builddir@/doc/MQTTClient/
	cd doc && $(DOXYGEN) DoxyfileV3ClientAPI
	echo Timestamp > DoxyfileV3ClientAPI.stamp

CLEANFILES  = DoxyfileV3ClientAPI.stamp

DoxyfileV3AsyncAPI.stamp:
	$(MKDIR_P) @top_builddir@/doc/MQTTAsync/
	cd doc && $(DOXYGEN) DoxyfileV3AsyncAPI
	echo Timestamp > DoxyfileV3AsyncAPI.stamp

CLEANFILES += DoxyfileV3AsyncAPI.stamp

DoxyfileV3ClientInternal.stamp:
	$(MKDIR_P) @top_builddir@/doc/MQTTClient_internal/
	cd doc && $(DOXYGEN) DoxyfileV3ClientInternal
	echo Timestamp > DoxyfileV3ClientInternal.stamp

CLEANFILES += DoxyfileV3ClientInternal.stamp

all-local: DoxyfileV3ClientAPI.stamp DoxyfileV3AsyncAPI.stamp DoxyfileV3ClientInternal.stamp
clean-local:
	rm -rf doc/docs/man
endif
