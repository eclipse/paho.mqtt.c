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

# Build output is produced in the current directory.

# MQTTCLIENT_DIR must point to the base directory containing the MQTT client source code.
# Example:  make -f makefile MQTTCLIENT_DIR=V3Client/MQTTClient

# Note: this makefile requires gnu make which you may need to obtain if compiling on AIX or Solaris
# Note: on OS X you should install XCode and the associated command-line tools

ifndef MQTTCLIENT_DIR
  MQTTCLIENT_DIR = $(CURDIR)
endif

ifndef SSL_DIR
  SSL_DIR = ${MQTTCLIENT_DIR}/../../../../source/openssl-1.0.1c/
endif

ifndef SAMPLES_DIR
SAMPLES_DIR = ${SSL_DIR}/../com.ibm.mq.mqxr.listener/SDK/clients/c/samples/
endif

MQTTLIB = mqttv3c
VPATH   = ${MQTTCLIENT_DIR}  
ALL_SOURCE_FILES = $(wildcard $(MQTTCLIENT_DIR)/*.c)
SOURCE_FILES = $(filter-out $(MQTTCLIENT_DIR)/MQTTAsync.c $(MQTTCLIENT_DIR)/MQTTVersion.c $(MQTTCLIENT_DIR)/SSLSocket.c, $(ALL_SOURCE_FILES))
ALL_HEADERS = $(MQTTCLIENT_DIR)/*.h
HEADERS = $(filter-out $(MQTTCLIENT_DIR)/MQTTAsync.h, $(ALL_HEADERS))

MQTTLIB_S = mqttv3cs
SOURCE_FILES_S = $(filter-out $(MQTTCLIENT_DIR)/MQTTAsync.c $(MQTTCLIENT_DIR)/MQTTVersion.c, $(ALL_SOURCE_FILES))

MQTTLIB_A = mqttv3a
SOURCE_FILES_A = $(filter-out $(MQTTCLIENT_DIR)/MQTTClient.c $(MQTTCLIENT_DIR)/MQTTVersion.c $(MQTTCLIENT_DIR)/SSLSocket.c, $(ALL_SOURCE_FILES))
HEADERS_A = $(MQTTCLIENT_DIR)/*.h

MQTTLIB_AS = mqttv3as
SOURCE_FILES_AS = $(filter-out $(MQTTCLIENT_DIR)/MQTTClient.c $(MQTTCLIENT_DIR)/MQTTVersion.c, $(ALL_SOURCE_FILES))

# In most cases the targets will be built for the native platform. However, to allow for cross compiling,
# a target platform variable can be set to override the platform choice. If this variable is set to
# a recognised string (currently just 'Arm') the appropriate values of OSTYPE and MACHINETYPE are set.
ifeq ($(TARGET_PLATFORM),Arm)
	OSTYPE = Linux
	MACHINETYPE = Arm
else
	ifeq ($(OS),Windows_NT)
		OSTYPE = $(OS)
	else
		OSTYPE = $(shell uname -s)
		MACHINETYPE = $(shell uname -m)
	endif
endif

ifeq ($(OSTYPE),Linux)

ifeq ($(MACHINETYPE),x86_64)
	CC = gcc

	CCFLAGS_SO = -fPIC -Os -Wall -I ${SSL_DIR}/include -fvisibility=hidden
	CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
	LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
	LDFLAGS_S = -shared -Wl,-soname,lib${MQTTLIB_S}.so -ldl -Wl,-whole-archive -L${SSL_DIR}/output/lib -lcrypto -lssl -Wl,-no-whole-archive
	LDFLAGS_A = -shared -Wl,-soname,lib${MQTTLIB_A}.so
	LDFLAGS_AS = -shared -Wl,-soname,lib${MQTTLIB_AS}.so -ldl -Wl,-whole-archive -L${SSL_DIR}/output/lib -lcrypto -lssl -Wl,-no-whole-archive
	LDFLAGS_EXE = -lpthread

	MQTTLIB_IA64 = linux_ia64/lib${MQTTLIB}.so
	MQTTLIB_IA64_S = linux_ia64/lib${MQTTLIB_S}.so
	MQTTLIB_IA64_A = linux_ia64/lib${MQTTLIB_A}.so
	MQTTLIB_IA64_AS = linux_ia64/lib${MQTTLIB_AS}.so

ifdef BUILD_SAMPLES
all: linux_ia64 ${MQTTLIB_IA64} ${MQTTLIB_IA64_A} ${MQTTLIB_IA64_S} ${MQTTLIB_IA64_AS} samples
else
all: linux_ia64 ${MQTTLIB_IA64} ${MQTTLIB_IA64_A} ${MQTTLIB_IA64_S} ${MQTTLIB_IA64_AS}
endif

linux_ia64:
	-mkdir linux_ia64

samples:
	-mkdir ${SAMPLES_DIR}/linux_ia64
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia64 -o ${SAMPLES_DIR}/linux_ia64/MQTTV3Sample ${SAMPLES_DIR}/MQTTV3Sample.c -lmqttv3c -lpthread
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia64 -o ${SAMPLES_DIR}/linux_ia64/MQTTV3SSample ${SAMPLES_DIR}/MQTTV3SSample.c -lmqttv3cs -lpthread
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia64 -o ${SAMPLES_DIR}/linux_ia64/MQTTV3ASample ${SAMPLES_DIR}/MQTTV3ASample.c -lmqttv3a -lpthread
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia64 -o ${SAMPLES_DIR}/linux_ia64/MQTTV3ASSample ${SAMPLES_DIR}/MQTTV3ASSample.c -lmqttv3as -lpthread

${MQTTLIB_IA64}: ${SOURCE_FILES} ${HEADERS}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@

${MQTTLIB_IA64_S}: ${SOURCE_FILES_S} ${HEADERS}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_S} -o $@ ${SOURCE_FILES_S} -DOPENSSL
	strip $@

${MQTTLIB_IA64_A}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_A} -o $@ ${SOURCE_FILES_A}
	strip $@

${MQTTLIB_IA64_AS}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_AS} -o $@ ${SOURCE_FILES_AS} -DOPENSSL
	strip $@

else
ifeq ($(MACHINETYPE),ppc64)
	CC = gcc

	CCFLAGS_SO = -fPIC -Os -Wall -I ${SSL_DIR}/include -fvisibility=hidden
	CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
	LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
	LDFLAGS_S = -shared -Wl,-soname,lib${MQTTLIB_S}.so -ldl -Wl,-whole-archive -L${SSL_DIR}/output/lib -lcrypto -lssl -Wl,-no-whole-archive
	LDFLAGS_A = -shared -Wl,-soname,lib${MQTTLIB_A}.so
	LDFLAGS_AS = -shared -Wl,-soname,lib${MQTTLIB_AS}.so -ldl -Wl,-whole-archive -L${SSL_DIR}/output/lib -lcrypto -lssl -Wl,-no-whole-archive
	LDFLAGS_EXE = -lpthread

	MQTTLIB_PPC64 = linux_ppc64/lib${MQTTLIB}.so
	MQTTLIB_PPC64_S = linux_ppc64/lib${MQTTLIB_S}.so
	MQTTLIB_PPC64_A = linux_ppc64/lib${MQTTLIB_A}.so
	MQTTLIB_PPC64_AS = linux_ppc64/lib${MQTTLIB_AS}.so

all: linux_ppc64 ${MQTTLIB_PPC64} ${MQTTLIB_PPC64_A} #${MQTTLIB_PPC64_S} ${MQTTLIB_PPC64_AS}

linux_ppc64:
	-mkdir linux_ppc64

${MQTTLIB_PPC64}: ${SOURCE_FILES} ${HEADERS}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@

${MQTTLIB_PPC64_S}: ${SOURCE_FILES_S} ${HEADERS}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_S} -o $@ ${SOURCE_FILES_S} -DOPENSSL
	strip $@

${MQTTLIB_PPC64_A}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_A} -o $@ ${SOURCE_FILES_A}
	strip $@

${MQTTLIB_PPC64_AS}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_AS} -o $@ ${SOURCE_FILES_AS} -DOPENSSL
	strip $@

else
ifeq ($(MACHINETYPE),s390x)
  CC 	   = gcc

  CCFLAGS_SO = -fPIC -Os -Wall -DREVERSED -fvisibility=hidden
  CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
  LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
  LDFLAGS_EXE = -lpthread

  MQTTLIB_390 = linux_s390x/lib${MQTTLIB}.so

  all: ${MQTTLIB_390}

${MQTTLIB_390}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_s390x
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@

else
ifeq ($(MACHINETYPE),Arm)

	CC = gcc
	# ARM_GLIBC_CC = arm-linux-gcc
	# ARM_UCLIBC_CC = ~/x-tools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-gcc

	ifndef ARM_GLIBC_CC
		ARM_GLIBC_CC = arm-linux-gcc
	endif
	ifndef ARM_UCLIBC_CC
		ARM_UCLIBC_CC = arm-unknown-linux-uclibcgnueabi-gcc
	endif

	ifndef ARM_GLIBC_STRIP
		ARM_GLIBC_STRIP = $(subst -gcc,-strip,${ARM_GLIBC_CC})
	endif
	ifndef ARM_UCLIBC_STRIP
		ARM_UCLIBC_STRIP = $(subst -gcc,-strip,${ARM_UCLIBC_CC})
	endif

	CCFLAGS_SO = -fPIC -Os -Wall -fvisibility=hidden
	CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
	LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
	LDFLAGS_EXE = -lpthread

	MQTTLIB_ARM_GLIBC = linux_ARM_glibc/lib${MQTTLIB}.so
	MQTTLIB_ARM_UCLIBC = linux_ARM_uclibc/lib${MQTTLIB}.so

	MQTTLIB_ARM_GLIBC_A = linux_ARM_glibc/lib${MQTTLIB_A}.so
	MQTTLIB_ARM_UCLIBC_A = linux_ARM_uclibc/lib${MQTTLIB_A}.so

all: ${MQTTLIB_ARM_GLIBC} ${MQTTLIB_ARM_UCLIBC}

${MQTTLIB_ARM_GLIBC}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_ARM_glibc
	${ARM_GLIBC_CC} ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	${ARM_GLIBC_STRIP} $@

${MQTTLIB_ARM_UCLIBC}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_ARM_uclibc
	${ARM_UCLIBC_CC} ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	${ARM_UCLIBC_STRIP} $@

else

	CC = gcc

	CCFLAGS_SO = -fPIC -Os -Wall -I ${SSL_DIR}/include -fvisibility=hidden
	CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
	LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
	LDFLAGS_S = -shared -Wl,-soname,lib${MQTTLIB_S}.so -ldl -Wl,-whole-archive -L${SSL_DIR}/output/lib -lcrypto -lssl -Wl,-no-whole-archive
	LDFLAGS_A = -shared -Wl,-soname,lib${MQTTLIB_A}.so
	LDFLAGS_AS = -shared -Wl,-soname,lib${MQTTLIB_AS}.so -ldl -Wl,-whole-archive -L/${SSL_DIR}/output/lib -lcrypto -lssl -Wl,-no-whole-archive
	LDFLAGS_EXE = -lpthread

	MQTTLIB_IA32 = linux_ia32/lib${MQTTLIB}.so
	MQTTLIB_IA32_S = linux_ia32/lib${MQTTLIB_S}.so
	MQTTLIB_IA32_A = linux_ia32/lib${MQTTLIB_A}.so
	MQTTLIB_IA32_AS = linux_ia32/lib${MQTTLIB_AS}.so

ifdef BUILD_SAMPLES
all: linux_ia32 ${MQTTLIB_IA32_A} ${MQTTLIB_IA32} ${MQTTLIB_IA32_S} ${MQTTLIB_IA32_AS} samples
else
all: linux_ia32 ${MQTTLIB_IA32_A} ${MQTTLIB_IA32} ${MQTTLIB_IA32_S} ${MQTTLIB_IA32_AS}
endif

samples:
	-mkdir ${SAMPLES_DIR}/linux_ia32
	${CC} -m32 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia32 -o ${SAMPLES_DIR}/linux_ia32/MQTTV3Sample ${SAMPLES_DIR}/MQTTV3Sample.c -lmqttv3c -lpthread
	${CC} -m32 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia32 -o ${SAMPLES_DIR}/linux_ia32/MQTTV3SSample ${SAMPLES_DIR}/MQTTV3SSample.c -lmqttv3cs -lpthread
	${CC} -m32 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia32 -o ${SAMPLES_DIR}/linux_ia32/MQTTV3ASample ${SAMPLES_DIR}/MQTTV3ASample.c -lmqttv3a -lpthread
	${CC} -m32 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/linux_ia32 -o ${SAMPLES_DIR}/linux_ia32/MQTTV3ASSample ${SAMPLES_DIR}/MQTTV3ASSample.c -lmqttv3as -lpthread

linux_ia32:
	-mkdir linux_ia32

${MQTTLIB_IA32}: ${SOURCE_FILES} ${HEADERS}
	${CC} -m32 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@

${MQTTLIB_IA32_S}: ${SOURCE_FILES_S} ${HEADERS}
	${CC} -m32 ${CCFLAGS_SO} ${LDFLAGS_S} -o $@ ${SOURCE_FILES_S} -DOPENSSL
	strip $@

${MQTTLIB_IA32_A}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} -m32 ${CCFLAGS_SO} ${LDFLAGS_A} -o $@ ${SOURCE_FILES_A}
	strip $@

${MQTTLIB_IA32_AS}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} -m32 ${CCFLAGS_SO} ${LDFLAGS_AS} -o $@ ${SOURCE_FILES_AS} -DOPENSSL
	strip $@
endif

endif

endif

endif

endif

ifeq ($(OSTYPE),Darwin)

ifeq ($(MACHINETYPE),x86_64)
	CC = gcc

	CCFLAGS_SO = -fPIC -Os -Wall -I ${SSL_DIR}/include
	CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
	LDFLAGS = -shared -Wl,-install_name,lib${MQTTLIB}.so
	LDFLAGS_S = -shared -Wl,-install_name,lib${MQTTLIB_S}.so -ldl -Wl,-all_load -L${SSL_DIR}/lib -lcrypto -lssl
	LDFLAGS_A = -shared -Wl,-install_name,lib${MQTTLIB_A}.so
	LDFLAGS_AS = -shared -Wl,-install_name,lib${MQTTLIB_AS}.so -ldl -Wl,-all_load -L${SSL_DIR}/lib -lcrypto -lssl
	LDFLAGS_EXE = -lpthread

	MQTTLIB_IA64 = darwin_ia64/lib${MQTTLIB}.so
	MQTTLIB_IA64_S = darwin_ia64/lib${MQTTLIB_S}.so
	MQTTLIB_IA64_A = darwin_ia64/lib${MQTTLIB_A}.so
	MQTTLIB_IA64_AS = darwin_ia64/lib${MQTTLIB_AS}.so

ifdef BUILD_SAMPLES
all: darwin_ia64 ${MQTTLIB_IA64} ${MQTTLIB_IA64_A} ${MQTTLIB_IA64_S} ${MQTTLIB_IA64_AS} samples
else
all: darwin_ia64 ${MQTTLIB_IA64} ${MQTTLIB_IA64_A} ${MQTTLIB_IA64_S} ${MQTTLIB_IA64_AS}
endif

darwin_ia64:
	-mkdir darwin_ia64

samples:
	-mkdir ${SAMPLES_DIR}/darwin_ia64
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/darwin_ia64 -o ${SAMPLES_DIR}/darwin_ia64/MQTTV3Sample ${SAMPLES_DIR}/MQTTV3Sample.c -lmqttv3c -lpthread
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/darwin_ia64 -o ${SAMPLES_DIR}/darwin_ia64/MQTTV3SSample ${SAMPLES_DIR}/MQTTV3SSample.c -lmqttv3cs -lpthread
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/darwin_ia64 -o ${SAMPLES_DIR}/darwin_ia64/MQTTV3ASample ${SAMPLES_DIR}/MQTTV3ASample.c -lmqttv3a -lpthread
	${CC} -m64 -I ${MQTTCLIENT_DIR} -L ${MQTTCLIENT_DIR}/darwin_ia64 -o ${SAMPLES_DIR}/darwin_ia64/MQTTV3ASSample ${SAMPLES_DIR}/MQTTV3ASSample.c -lmqttv3as -lpthread

${MQTTLIB_IA64}: ${SOURCE_FILES} ${HEADERS}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip -x $@

${MQTTLIB_IA64_S}: ${SOURCE_FILES_S} ${HEADERS}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_S} -o $@ ${SOURCE_FILES_S} -DOPENSSL
	strip -x $@

${MQTTLIB_IA64_A}: ${SOURCE_FILES_A} ${HEADERS_A}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_A} -o $@ ${SOURCE_FILES_A}
	strip -x $@

${MQTTLIB_IA64_AS}: ${SOURCE_FILES_AS} ${HEADERS_A}
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS_AS} -o $@ ${SOURCE_FILES_AS} -DOPENSSL
	strip -x $@

endif

endif

ifeq ($(OSTYPE),AIX)
# applications must be linked with -I. -L. -lmqttv3c -Wl,-brtl -pthread
  CC 	   = gxlc

  CCFLAGS_SO = -fPIC -Os -shared -DREVERSED -DAIX
  LDFLAGS = -Wl,-G

  MQTTLIB_AIX = aix/lib${MQTTLIB}.so

all: ${MQTTLIB_AIX}

${MQTTLIB_AIX}: ${SOURCE_FILES} ${HEADERS}
	-mkdir aix
	${CC} ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@
endif


ifeq ($(OSTYPE),Windows_NT)
  CC       = cl
  LD 	   = link

  # compiler flags
  CPPFLAGS = /D "WIN32" /D "_UNICODE" /D "UNICODE" /D "_CRT_SECURE_NO_WARNINGS"
  CFLAGS = /nologo /c /O2 /W3 /Fd /MD /TC
  CPPFLAGS_S = /D "OPENSSL" /I ${SSL_DIR}/include

  # linker flags
  LINKFLAGS = /nologo /machine:x86 /manifest
  WINLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib
  WINLIBS_S = crypt32.lib ssleay32.lib libeay32.lib
  SSLLIBPATH = /LIBPATH:${SSL_DIR}/output/lib
  # Flags to link C libraries
  LIBLINKFLAGS = /dll
  IMP = /implib:$(@:.dll=.lib)
  LIBPDB = /pdb:$(@:.dll=.pdb)
  LIBMAP = /map:$(@:.dll=.map)
  LIBLDFLAGS = $(LINKFLAGS) $(WINLIBS) $(LIBLINKFLAGS) $(IMP) $(LIBPDB) $(LIBMAP)
  LIBLDFLAGS_S = $(LINKFLAGS) $(SSLLIBPATH) $(WINLIBS) $(WINLIBS_S) $(LIBLINKFLAGS) $(IMP) $(LIBPDB) $(LIBMAP)
  # Flags to link C programs
  MAINLINKFLAGS = /subsystem:console
  EXEPDB = /pdb:$(@:.exe=.pdb)
  EXEMAP = /map:$(@:.exe=.map)
  MAINLDFLAGS = $(LINKFLAGS) $(WINLIBS) $(MAINLINKFLAGS) $(EXEPDB) $(EXEMAP)

  MQTTLDLIB = windows_ia32/$(MQTTLIB).lib
  MQTTDLL   = windows_ia32/$(MQTTLIB).dll
  MQTTLDLIB_A = windows_ia32/$(MQTTLIB_A).lib
  MQTTDLL_A   = windows_ia32/$(MQTTLIB_A).dll
  MQTTLDLIB_AS = windows_ia32/$(MQTTLIB_AS).lib
  MQTTDLL_AS   = windows_ia32/$(MQTTLIB_AS).dll
  MQTTLDLIB_S = windows_ia32/$(MQTTLIB_S).lib
  MQTTDLL_S   = windows_ia32/$(MQTTLIB_S).dll

ifdef BUILD_SAMPLES
all: windows_ia32 ${MQTTDLL} ${MQTTDLL_A} ${MQTTDLL_AS} ${MQTTDLL_S} samples
else
all: windows_ia32 ${MQTTDLL} ${MQTTDLL_A} ${MQTTDLL_AS} ${MQTTDLL_S}
endif

samples:
	-mkdir ${SAMPLES_DIR}/windows_ia32
	${CC} /nologo /D "WIN32" /I ${MQTTCLIENT_DIR} ${SAMPLES_DIR}/MQTTV3Sample.c /Fe${SAMPLES_DIR}/windows_ia32/ /link /LIBPATH:${MQTTCLIENT_DIR}/windows_ia32 /nologo /subsystem:console /manifest /machine:x86 mqttv3c.lib
	${CC} /nologo /D "WIN32" /I ${MQTTCLIENT_DIR} ${SAMPLES_DIR}/MQTTV3CSSample.c /Fe${SAMPLES_DIR}/windows_ia32/ /link /LIBPATH:${MQTTCLIENT_DIR}/windows_ia32 /nologo /subsystem:console /manifest /machine:x86 mqttv3cs.lib
	${CC} /nologo /D "WIN32" /I ${MQTTCLIENT_DIR} ${SAMPLES_DIR}/MQTTV3ASample.c /Fe${SAMPLES_DIR}/windows_ia32/ /link /LIBPATH:${MQTTCLIENT_DIR}/windows_ia32 /nologo /subsystem:console /manifest /machine:x86 mqttv3a.lib
	${CC} /nologo /D "WIN32" /I ${MQTTCLIENT_DIR} ${SAMPLES_DIR}/MQTTV3ASSample.c /Fe${SAMPLES_DIR}/windows_ia32/ /link /LIBPATH:${MQTTCLIENT_DIR}/windows_ia32 /nologo /subsystem:console /manifest /machine:x86 mqttv3as.lib

windows_ia32:
	-mkdir windows_ia32

$(MQTTDLL): ${SOURCE_FILES}  ${HEADERS}
	-rm -f *.obj
	${CC} ${CPPFLAGS} ${CFLAGS} /I ${MQTTCLIENT_DIR} /I ${MQTTCLIENT_DIR}/.. /Fo ${SOURCE_FILES}
	$(LD) $(LIBLDFLAGS) *.obj /out:$(MQTTDLL)
	mt -manifest windows_ia32/mqttv3c.dll.manifest -outputresource:$(MQTTDLL)\;2

$(MQTTDLL_A): ${SOURCE_FILES_A}  ${HEADERS_A}
	-rm -f *.obj
	${CC} ${CPPFLAGS} ${CFLAGS} /I ${MQTTCLIENT_DIR} /I ${MQTTCLIENT_DIR}/.. /Fo ${SOURCE_FILES_A}
	$(LD) $(LIBLDFLAGS) *.obj /out:$(MQTTDLL_A)
	mt -manifest windows_ia32/mqttv3a.dll.manifest -outputresource:$(MQTTDLL_A)\;2

$(MQTTDLL_S): ${SOURCE_FILES_S}  ${HEADERS}
	-rm -f *.obj
	${CC} ${CPPFLAGS} ${CFLAGS} ${CPPFLAGS_S} /I ${MQTTCLIENT_DIR} /I ${MQTTCLIENT_DIR}/.. /Fo ${SOURCE_FILES_S}
	$(LD) $(LIBLDFLAGS_S) *.obj /out:$(MQTTDLL_S)
	mt -manifest windows_ia32/mqttv3cs.dll.manifest -outputresource:$(MQTTDLL_S)\;2

$(MQTTDLL_AS): ${SOURCE_FILES_AS}  ${HEADERS}
	-rm -f *.obj
	${CC} ${CPPFLAGS} ${CFLAGS} ${CPPFLAGS_S} /I ${MQTTCLIENT_DIR} /I ${MQTTCLIENT_DIR}/.. /Fo ${SOURCE_FILES_AS}
	$(LD) $(LIBLDFLAGS_S) *.obj /out:$(MQTTDLL_AS)
	mt -manifest windows_ia32/mqttv3as.dll.manifest -outputresource:$(MQTTDLL_AS)\;2

endif

#########################################################################################################
# The following was how the C Clients were built using cygwin GNU make and has been left here as a reference
# This has been converted to use ming GNU make (which uses the native format of file names)
#
#$(MQTTDLL): $(shell cygpath -au ${SOURCE_FILES})  $(shell cygpath -au ${HEADERS})
#	-mkdir windows_ia32
#	${CC} ${CPPFLAGS} ${CFLAGS} /I $(shell cygpath -am ${MQTTCLIENT_DIR}) /I $(shell cygpath -am ${MQTTCLIENT_DIR})/.. /Fo $(shell cygpath -am ${SOURCE_FILES})
#	$(LD) $(LIBLDFLAGS) *.obj /out:$(MQTTDLL)
#	mt -manifest windows_ia32/mqttv3c.dll.manifest -outputresource:$(MQTTDLL)\;2
#
#endif
#########################################################################################################
