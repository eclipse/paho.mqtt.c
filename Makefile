#*******************************************************************************
#  Copyright (c) 2009, 2014 IBM Corp.
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
#     Ian Craggs - initial API and implementation and/or initial documentation
#     Allan Stockdill-Mander - SSL updates
#     Andy Piper - various fixes
#     Ian Craggs - OSX build
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

SHELL = /bin/sh
.PHONY: clean, mkdir, install, uninstall, html 

# assume this is normally run in the main Paho directory
ifndef srcdir
  srcdir = src
endif

ifndef blddir
  blddir = build/output
endif

ifndef prefix
	prefix = /usr/local
endif

ifndef exec_prefix
	exec_prefix = ${prefix}
endif

bindir = $(exec_prefix)/bin
includedir = $(prefix)/include
libdir = $(exec_prefix)/lib

SOURCE_FILES = $(wildcard $(srcdir)/*.c)
SOURCE_FILES_C = $(filter-out $(srcdir)/MQTTAsync.c $(srcdir)/MQTTVersion.c $(srcdir)/SSLSocket.c, $(SOURCE_FILES))
SOURCE_FILES_CS = $(filter-out $(srcdir)/MQTTAsync.c $(srcdir)/MQTTVersion.c, $(SOURCE_FILES))
SOURCE_FILES_A = $(filter-out $(srcdir)/MQTTClient.c $(srcdir)/MQTTVersion.c $(srcdir)/SSLSocket.c, $(SOURCE_FILES))
SOURCE_FILES_AS = $(filter-out $(srcdir)/MQTTClient.c $(srcdir)/MQTTVersion.c, $(SOURCE_FILES))

HEADERS = $(srcdir)/*.h
HEADERS_C = $(filter-out $(srcdir)/MQTTAsync.h, $(HEADERS))
HEADERS_A = $(HEADERS)

SAMPLE_FILES_C = stdinpub stdoutsub pubsync pubasync subasync
SYNC_SAMPLES = ${addprefix ${blddir}/samples/,${SAMPLE_FILES_C}}

SAMPLE_FILES_A = stdoutsuba MQTTAsync_subscribe MQTTAsync_publish
ASYNC_SAMPLES = ${addprefix ${blddir}/samples/,${SAMPLE_FILES_A}}

TEST_FILES_C = test1 sync_client_test test_mqtt4sync
SYNC_TESTS = ${addprefix ${blddir}/test/,${TEST_FILES_C}}

TEST_FILES_CS = test3
SYNC_SSL_TESTS = ${addprefix ${blddir}/test/,${TEST_FILES_CS}}

TEST_FILES_A = test4 test_mqtt4async
ASYNC_TESTS = ${addprefix ${blddir}/test/,${TEST_FILES_A}}

TEST_FILES_AS = test5
ASYNC_SSL_TESTS = ${addprefix ${blddir}/test/,${TEST_FILES_AS}}

# The names of the four different libraries to be built
MQTTLIB_C = paho-mqtt3c
MQTTLIB_CS = paho-mqtt3cs
MQTTLIB_A = paho-mqtt3a
MQTTLIB_AS = paho-mqtt3as

# determine current platform
ifeq ($(OS),Windows_NT)
	OSTYPE = $(OS)
else
	OSTYPE = $(shell uname -s)
	MACHINETYPE = $(shell uname -m)
endif

ifeq ($(OSTYPE),Linux)

CC ?= gcc

ifndef INSTALL
INSTALL = install
endif
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA =  $(INSTALL) -m 644
DOXYGEN_COMMAND = doxygen

MAJOR_VERSION = 1
MINOR_VERSION = 0
VERSION = ${MAJOR_VERSION}.${MINOR_VERSION}

MQTTLIB_C_TARGET = ${blddir}/lib${MQTTLIB_C}.so.${VERSION}
MQTTLIB_CS_TARGET = ${blddir}/lib${MQTTLIB_CS}.so.${VERSION}
MQTTLIB_A_TARGET = ${blddir}/lib${MQTTLIB_A}.so.${VERSION}
MQTTLIB_AS_TARGET = ${blddir}/lib${MQTTLIB_AS}.so.${VERSION}
MQTTVERSION_TARGET = ${blddir}/MQTTVersion

CCFLAGS_SO = -g -fPIC -Os -Wall -fvisibility=hidden
FLAGS_EXE = -I ${srcdir} -lpthread -L ${blddir}

LDFLAGS_C = -shared -Wl,-soname,lib$(MQTTLIB_C).so.${MAJOR_VERSION} -Wl,-init,MQTTClient_init -lpthread 
LDFLAGS_CS = -shared -Wl,-soname,lib$(MQTTLIB_CS).so.${MAJOR_VERSION} -lpthread -ldl -lssl -lcrypto -Wl,-no-whole-archive -Wl,-init,MQTTClient_init 
LDFLAGS_A = -shared -Wl,-soname,lib${MQTTLIB_A}.so.${MAJOR_VERSION} -Wl,-init,MQTTAsync_init -lpthread
LDFLAGS_AS = -shared -Wl,-soname,lib${MQTTLIB_AS}.so.${MAJOR_VERSION} -lpthread -ldl -lssl -lcrypto -Wl,-no-whole-archive -Wl,-init,MQTTAsync_init 

all: build
	
build: | mkdir ${MQTTLIB_C_TARGET} ${MQTTLIB_CS_TARGET} ${MQTTLIB_A_TARGET} ${MQTTLIB_AS_TARGET} ${MQTTVERSION_TARGET} ${SYNC_SAMPLES} ${ASYNC_SAMPLES} ${SYNC_TESTS} ${SYNC_SSL_TESTS} ${ASYNC_TESTS} ${ASYNC_SSL_TESTS}

clean:
	rm -rf ${blddir}/*
	
mkdir:
	-mkdir -p ${blddir}/samples
	-mkdir -p ${blddir}/test

${SYNC_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_C} ${FLAGS_EXE}

${SYNC_SSL_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_CS} ${FLAGS_EXE} -lssl

${ASYNC_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_A} ${FLAGS_EXE} 

${ASYNC_SSL_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_AS} ${FLAGS_EXE} -lssl

${SYNC_SAMPLES}: ${blddir}/samples/%: ${srcdir}/samples/%.c
	${CC} -o ${blddir}/samples/${basename ${+F}} $< -l${MQTTLIB_C} ${FLAGS_EXE}

${ASYNC_SAMPLES}: ${blddir}/samples/%: ${srcdir}/samples/%.c
	${CC} -o ${blddir}/samples/${basename ${+F}} $< -l${MQTTLIB_A} ${FLAGS_EXE}

${MQTTLIB_C_TARGET}: ${SOURCE_FILES_C} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_C} ${LDFLAGS_C}
	-ln -s lib$(MQTTLIB_C).so.${VERSION}  ${blddir}/lib$(MQTTLIB_C).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_C).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_C).so

${MQTTLIB_CS_TARGET}: ${SOURCE_FILES_CS} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_CS} -DOPENSSL ${LDFLAGS_CS}
	-ln -s lib$(MQTTLIB_CS).so.${VERSION}  ${blddir}/lib$(MQTTLIB_CS).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_CS).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_CS).so

${MQTTLIB_A_TARGET}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_A} ${LDFLAGS_A}
	-ln -s lib$(MQTTLIB_A).so.${VERSION}  ${blddir}/lib$(MQTTLIB_A).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_A).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_A).so

${MQTTLIB_AS_TARGET}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_AS} -DOPENSSL ${LDFLAGS_AS}
	-ln -s lib$(MQTTLIB_AS).so.${VERSION}  ${blddir}/lib$(MQTTLIB_AS).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_AS).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_AS).so

${MQTTVERSION_TARGET}: $(srcdir)/MQTTVersion.c $(srcdir)/MQTTAsync.h
	${CC} ${FLAGS_EXE} -o $@ -l${MQTTLIB_A} $(srcdir)/MQTTVersion.c -ldl

strip_options:
	$(eval INSTALL_OPTS := -s)

install-strip: build strip_options install

install: build 
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_C_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_CS_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_A_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_AS_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_PROGRAM) ${INSTALL_OPTS} ${MQTTVERSION_TARGET} $(DESTDIR)${bindir}
	/sbin/ldconfig $(DESTDIR)${libdir}
	ln -s lib$(MQTTLIB_C).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_C).so
	ln -s lib$(MQTTLIB_CS).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_CS).so
	ln -s lib$(MQTTLIB_A).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_A).so
	ln -s lib$(MQTTLIB_AS).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_AS).so
	$(INSTALL_DATA) ${srcdir}/MQTTAsync.h $(DESTDIR)${includedir}
	$(INSTALL_DATA) ${srcdir}/MQTTClient.h $(DESTDIR)${includedir}
	$(INSTALL_DATA) ${srcdir}/MQTTClientPersistence.h $(DESTDIR)${includedir}

uninstall:
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_C).so.${VERSION}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_CS).so.${VERSION}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_A).so.${VERSION}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_AS).so.${VERSION}
	rm $(DESTDIR)${bindir}/MQTTVersion
	/sbin/ldconfig $(DESTDIR)${libdir}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_C).so
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_CS).so
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_A).so
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_AS).so
	rm $(DESTDIR)${includedir}/MQTTAsync.h
	rm $(DESTDIR)${includedir}/MQTTClient.h 
	rm $(DESTDIR)${includedir}/MQTTClientPersistence.h 

html:
	-mkdir -p ${blddir}/doc
	cd ${srcdir}; $(DOXYGEN_COMMAND) ../doc/DoxyfileV3ClientAPI
	cd ${srcdir}; $(DOXYGEN_COMMAND) ../doc/DoxyfileV3AsyncAPI
	cd ${srcdir}; $(DOXYGEN_COMMAND) ../doc/DoxyfileV3ClientInternal

endif



ifeq ($(OSTYPE),Darwin)

CC ?= gcc

ifndef INSTALL
INSTALL = install
endif
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA =  $(INSTALL) -m 644
DOXYGEN_COMMAND = doxygen

MAJOR_VERSION = 1
MINOR_VERSION = 0
VERSION = ${MAJOR_VERSION}.${MINOR_VERSION}

MQTTLIB_C_TARGET = ${blddir}/lib${MQTTLIB_C}.so.${VERSION}
MQTTLIB_CS_TARGET = ${blddir}/lib${MQTTLIB_CS}.so.${VERSION}
MQTTLIB_A_TARGET = ${blddir}/lib${MQTTLIB_A}.so.${VERSION}
MQTTLIB_AS_TARGET = ${blddir}/lib${MQTTLIB_AS}.so.${VERSION}
MQTTVERSION_TARGET = ${blddir}/MQTTVersion

CCFLAGS_SO = -g -fPIC -Os -Wall -fvisibility=hidden -Wno-deprecated-declarations -DUSE_NAMED_SEMAPHORES
FLAGS_EXE = -I ${srcdir} -lpthread -L ${blddir}

LDFLAGS_C = -shared -Wl,-install_name,lib$(MQTTLIB_C).so.${MAJOR_VERSION} -Wl,-init,_MQTTClient_init -lpthread 
LDFLAGS_CS = -shared -Wl,-install_name,lib$(MQTTLIB_CS).so.${MAJOR_VERSION} -lpthread -ldl -lssl -lcrypto -Wl,-init,_MQTTClient_init 
LDFLAGS_A = -shared -Wl,-install_name,lib${MQTTLIB_A}.so.${MAJOR_VERSION} -Wl,-init,_MQTTAsync_init -lpthread
LDFLAGS_AS = -shared -Wl,-install_name,lib${MQTTLIB_AS}.so.${MAJOR_VERSION} -lpthread -ldl -lssl -lcrypto -Wl,-init,_MQTTAsync_init 

all: build
	
build: | mkdir ${MQTTLIB_C_TARGET} ${MQTTLIB_CS_TARGET} ${MQTTLIB_A_TARGET} ${MQTTLIB_AS_TARGET} ${MQTTVERSION_TARGET} ${SYNC_SAMPLES} ${ASYNC_SAMPLES} ${SYNC_TESTS} ${SYNC_SSL_TESTS} ${ASYNC_TESTS} ${ASYNC_SSL_TESTS}

clean:
	rm -rf ${blddir}/*
	
mkdir:
	-mkdir -p ${blddir}/samples
	-mkdir -p ${blddir}/test

${SYNC_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_C} ${FLAGS_EXE}

${SYNC_SSL_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_CS} ${FLAGS_EXE} -lssl

${ASYNC_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_A} ${FLAGS_EXE} 

${ASYNC_SSL_TESTS}: ${blddir}/test/%: ${srcdir}/../test/%.c
	${CC} -g -o ${blddir}/test/${basename ${+F}} $< -l${MQTTLIB_AS} ${FLAGS_EXE} -lssl

${SYNC_SAMPLES}: ${blddir}/samples/%: ${srcdir}/samples/%.c
	${CC} -o ${blddir}/samples/${basename ${+F}} $< -l${MQTTLIB_C} ${FLAGS_EXE}

${ASYNC_SAMPLES}: ${blddir}/samples/%: ${srcdir}/samples/%.c
	${CC} -o ${blddir}/samples/${basename ${+F}} $< -l${MQTTLIB_A} ${FLAGS_EXE}

${MQTTLIB_C_TARGET}: ${SOURCE_FILES_C} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_C} ${LDFLAGS_C}
	-ln -s lib$(MQTTLIB_C).so.${VERSION}  ${blddir}/lib$(MQTTLIB_C).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_C).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_C).so

${MQTTLIB_CS_TARGET}: ${SOURCE_FILES_CS} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_CS} -DOPENSSL ${LDFLAGS_CS}
	-ln -s lib$(MQTTLIB_CS).so.${VERSION}  ${blddir}/lib$(MQTTLIB_CS).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_CS).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_CS).so

${MQTTLIB_A_TARGET}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_A} ${LDFLAGS_A}
	-ln -s lib$(MQTTLIB_A).so.${VERSION}  ${blddir}/lib$(MQTTLIB_A).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_A).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_A).so

${MQTTLIB_AS_TARGET}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_AS} -DOPENSSL ${LDFLAGS_AS}
	-ln -s lib$(MQTTLIB_AS).so.${VERSION}  ${blddir}/lib$(MQTTLIB_AS).so.${MAJOR_VERSION}
	-ln -s lib$(MQTTLIB_AS).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_AS).so

${MQTTVERSION_TARGET}: $(srcdir)/MQTTVersion.c $(srcdir)/MQTTAsync.h
	${CC} ${FLAGS_EXE} -o $@ -l${MQTTLIB_A} $(srcdir)/MQTTVersion.c -ldl

strip_options:
	$(eval INSTALL_OPTS := -s)

install-strip: build strip_options install

install: build 
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_C_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_CS_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_A_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_DATA) ${INSTALL_OPTS} ${MQTTLIB_AS_TARGET} $(DESTDIR)${libdir}
	$(INSTALL_PROGRAM) ${INSTALL_OPTS} ${MQTTVERSION_TARGET} $(DESTDIR)${bindir}
	/sbin/ldconfig $(DESTDIR)${libdir}
	ln -s lib$(MQTTLIB_C).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_C).so
	ln -s lib$(MQTTLIB_CS).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_CS).so
	ln -s lib$(MQTTLIB_A).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_A).so
	ln -s lib$(MQTTLIB_AS).so.${MAJOR_VERSION} $(DESTDIR)${libdir}/lib$(MQTTLIB_AS).so
	$(INSTALL_DATA) ${srcdir}/MQTTAsync.h $(DESTDIR)${includedir}
	$(INSTALL_DATA) ${srcdir}/MQTTClient.h $(DESTDIR)${includedir}
	$(INSTALL_DATA) ${srcdir}/MQTTClientPersistence.h $(DESTDIR)${includedir}

uninstall:
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_C).so.${VERSION}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_CS).so.${VERSION}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_A).so.${VERSION}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_AS).so.${VERSION}
	rm $(DESTDIR)${bindir}/MQTTVersion
	/sbin/ldconfig $(DESTDIR)${libdir}
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_C).so
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_CS).so
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_A).so
	rm $(DESTDIR)${libdir}/lib$(MQTTLIB_AS).so
	rm $(DESTDIR)${includedir}/MQTTAsync.h
	rm $(DESTDIR)${includedir}/MQTTClient.h 
	rm $(DESTDIR)${includedir}/MQTTClientPersistence.h 

html:
	-mkdir -p ${blddir}/doc
	cd ${srcdir}; $(DOXYGEN_COMMAND) ../doc/DoxyfileV3ClientAPI
	cd ${srcdir}; $(DOXYGEN_COMMAND) ../doc/DoxyfileV3AsyncAPI
	cd ${srcdir}; $(DOXYGEN_COMMAND) ../doc/DoxyfileV3ClientInternal

endif
