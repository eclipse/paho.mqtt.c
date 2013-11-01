#*******************************************************************************
#  Copyright (c) 2009, 2013 IBM Corp.
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

CC = gcc

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

LDFLAGS_C = -shared -Wl,-soname,lib$(MQTTLIB_C).so.${MAJOR_VERSION}
LDFLAGS_CS = -shared -Wl,-soname,lib$(MQTTLIB_CS).so.${MAJOR_VERSION} -ldl -Wl,-whole-archive -lcrypto -lssl -Wl,-no-whole-archive
LDFLAGS_A = -shared -Wl,-soname,lib${MQTTLIB_A}.so.${MAJOR_VERSION}
LDFLAGS_AS = -shared -Wl,-soname,lib${MQTTLIB_AS}.so.${MAJOR_VERSION} -ldl -Wl,-whole-archive -lcrypto -lssl -Wl,-no-whole-archive

all: build
	
build: | mkdir ${MQTTLIB_C_TARGET} ${MQTTLIB_CS_TARGET} ${MQTTLIB_A_TARGET} ${MQTTLIB_AS_TARGET} ${MQTTVERSION_TARGET} ${SYNC_SAMPLES} ${ASYNC_SAMPLES}

clean:
	rm -rf ${blddir}/*
	
mkdir:
	-mkdir -p ${blddir}/samples

${SYNC_SAMPLES}: ${blddir}/samples/%: ${srcdir}/samples/%.c
	${CC} ${FLAGS_EXE} -o ${blddir}/samples/${basename ${+F}} $< -l${MQTTLIB_C} 

${ASYNC_SAMPLES}: ${blddir}/samples/%: ${srcdir}/samples/%.c
	${CC} ${FLAGS_EXE} -o ${blddir}/samples/${basename ${+F}} $< -l${MQTTLIB_A} 

${MQTTLIB_C_TARGET}: ${SOURCE_FILES_C} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} ${LDFLAGS_C} -o $@ ${SOURCE_FILES_C}
	ln -s lib$(MQTTLIB_C).so.${VERSION}  ${blddir}/lib$(MQTTLIB_C).so.${MAJOR_VERSION}
	ln -s lib$(MQTTLIB_C).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_C).so

${MQTTLIB_CS_TARGET}: ${SOURCE_FILES_CS} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} ${LDFLAGS_CS} -o $@ ${SOURCE_FILES_CS} -DOPENSSL
	ln -s lib$(MQTTLIB_CS).so.${VERSION}  ${blddir}/lib$(MQTTLIB_CS).so.${MAJOR_VERSION}
	ln -s lib$(MQTTLIB_CS).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_CS).so

${MQTTLIB_A_TARGET}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} ${CCFLAGS_SO} ${LDFLAGS_A} -o $@ ${SOURCE_FILES_A}
	ln -s lib$(MQTTLIB_A).so.${VERSION}  ${blddir}/lib$(MQTTLIB_A).so.${MAJOR_VERSION}
	ln -s lib$(MQTTLIB_A).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_A).so

${MQTTLIB_AS_TARGET}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} ${CCFLAGS_SO} ${LDFLAGS_AS} -o $@ ${SOURCE_FILES_AS} -DOPENSSL
	ln -s lib$(MQTTLIB_AS).so.${VERSION}  ${blddir}/lib$(MQTTLIB_AS).so.${MAJOR_VERSION}
	ln -s lib$(MQTTLIB_AS).so.${MAJOR_VERSION} ${blddir}/lib$(MQTTLIB_AS).so

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

