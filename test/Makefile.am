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
bin_PROGRAMS += test1
bin_PROGRAMS += test2
bin_PROGRAMS += test_mqtt4sync
bin_PROGRAMS += sync_client_test
endif

if PAHO_BUILD_ASYNC
bin_PROGRAMS += test4
bin_PROGRAMS += test6
bin_PROGRAMS += test8
bin_PROGRAMS += test_mqtt4async
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
bin_PROGRAMS += test3
endif

if PAHO_BUILD_ASYNC
bin_PROGRAMS += test5
endif

endif

###############################################################################
# Source code
###############################################################################

if PAHO_BUILD_SYNC
test1_SOURCES = test1.c

test2_SOURCES = test2.c

test_mqtt4sync_SOURCES = test_mqtt4sync.c

sync_client_test_SOURCES = sync_client_test.c
endif

if PAHO_BUILD_ASYNC
test4_SOURCES = test4.c

test6_SOURCES = test6.c

test8_SOURCES = test8.c

test_mqtt4async_SOURCES = test_mqtt4async.c
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
test3_SOURCES = test3.c
endif

if PAHO_BUILD_ASYNC
test5_SOURCES = test5.c
endif

endif

###############################################################################
# Compiler flags
###############################################################################

COMMONCFLAGS  = $(CFLAGS)
COMMONCFLAGS += -I$(top_srcdir)/src/

if PAHO_BUILD_SYNC
test1_CFLAGS = ${COMMONCFLAGS}

test2_CFLAGS = ${COMMONCFLAGS}

test_mqtt4sync_CFLAGS = ${COMMONCFLAGS}

sync_client_test_CFLAGS = ${COMMONCFLAGS}
endif

if PAHO_BUILD_ASYNC
test4_CFLAGS = ${COMMONCFLAGS}

test6_CFLAGS = ${COMMONCFLAGS}

test8_CFLAGS = ${COMMONCFLAGS}

test_mqtt4async_CFLAGS = ${COMMONCFLAGS}
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
test3_CFLAGS = ${COMMONCFLAGS}
endif

if PAHO_BUILD_ASYNC
test5_CFLAGS = ${COMMONCFLAGS}
endif

endif

###############################################################################
# Linker flags
###############################################################################

if PAHO_BUILD_SYNC
MQTTLIB_C = paho-mqtt3c

test1_LDFLAGS = $(LDFLAGS)
test1_LDADD  = $(LIBS)
test1_LDADD += ../src/libpaho-mqtt3c.la

test2_LDFLAGS = $(LDFLAGS)
test2_LDADD  = $(LIBS)
test2_LDADD += ../src/libpaho-mqtt3c.la

test_mqtt4sync_LDFLAGS = $(LDFLAGS)
test_mqtt4sync_LDADD  = $(LIBS)
test_mqtt4sync_LDADD += ../src/libpaho-mqtt3c.la

sync_client_test_LDFLAGS = $(LDFLAGS)
sync_client_test_LDADD  = $(LIBS)
sync_client_test_LDADD += ../src/libpaho-mqtt3c.la
endif

if PAHO_BUILD_ASYNC
MQTTLIB_A = paho-mqtt3a

test4_LDFLAGS = $(LDFLAGS)
test4_LDADD  = $(LIBS)
test4_LDADD += ../src/libpaho-mqtt3a.la

test6_LDFLAGS = $(LDFLAGS)
test6_LDADD  = $(LIBS)
test6_LDADD += ../src/libpaho-mqtt3a.la

test8_LDFLAGS = $(LDFLAGS)
test8_LDADD  = $(LIBS)
test8_LDADD += ../src/libpaho-mqtt3a.la

test_mqtt4async_LDFLAGS = $(LDFLAGS)
test_mqtt4async_LDADD  = $(LIBS)
test_mqtt4async_LDADD += ../src/libpaho-mqtt3a.la
endif

if PAHO_WITH_SSL

if PAHO_BUILD_SYNC
test3_LDFLAGS = $(LDFLAGS)
test3_LDADD  = $(LIBS)
test3_LDADD += ../src/libpaho-mqtt3cs.la
endif

if PAHO_BUILD_ASYNC
test5_LDFLAGS = $(LDFLAGS)
test5_LDADD  = $(LIBS)
test5_LDADD += ../src/libpaho-mqtt3as.la
endif

endif
