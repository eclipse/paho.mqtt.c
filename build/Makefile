# Build output is produced in the current directory.
# MQTTCLIENT_DIR must point to the base directory containing the MQTT client source code.
# Example:  make -f makefile MQTTCLIENT_DIR=V3Client/MQTTClient
# Note: this makefile requires gnu make which you may need to obtain if compiling on AIX or Solaris
ifndef MQTTCLIENT_DIR
  MQTTCLIENT_DIR = $(CURDIR)
endif

MQTTLIB = mqttv3c
VPATH   = ${MQTTCLIENT_DIR}  
SOURCE_FILES = $(MQTTCLIENT_DIR)/*.c
HEADERS = $(MQTTCLIENT_DIR)/*.h

ifeq ($(OS),Windows_NT)   
  OSTYPE = $(OS)
else
  OSTYPE = $(shell uname -s)
  MACHINETYPE = $(shell uname -m)
endif


ifeq ($(OSTYPE),Linux)

ifeq ($(MACHINETYPE),x86_64)
  CC 	   = gcc

  CCFLAGS_SO = -fPIC -Os -Wall
  CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
  LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
  LDFLAGS_EXE = -lpthread

  MQTTLIB_IA64 = linux_ia64/lib${MQTTLIB}.so
  
  all: ${MQTTLIB_IA64}

${MQTTLIB_IA64}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_ia64
	${CC} -m64 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@
	
else 
ifeq ($(MACHINETYPE),s390x)
  CC 	   = gcc

  CCFLAGS_SO = -fPIC -Os -Wall -DREVERSED
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
  CC 	   = gcc
  ARM_GLIBC_PREFIX = arm-linux-
  ARM_UCLIBC_PREFIX = ~/x-tools/arm-unknown-linux-uclibcgnueabi/bin/arm-unknown-linux-uclibcgnueabi-

  CCFLAGS_SO = -fPIC -Os -Wall
  CCFLAGS_EXE = -I ${MQTTCLIENT_DIR}
  LDFLAGS = -shared -Wl,-soname,lib${MQTTLIB}.so
  LDFLAGS_EXE = -lpthread

  MQTTLIB_IA32 = linux_ia32/lib${MQTTLIB}.so
  MQTTLIB_ARM_GLIBC = linux_ARM_glibc/lib${MQTTLIB}.so
  MQTTLIB_ARM_UCLIBC = linux_ARM_uclibc/lib${MQTTLIB}.so

all: ${MQTTLIB_IA32} ${MQTTLIB_ARM_GLIBC} ${MQTTLIB_ARM_UCLIBC}

${MQTTLIB_IA32}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_ia32
	${CC} -m32 ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	strip $@
	
${MQTTLIB_ARM_GLIBC}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_ARM_glibc
	${ARM_GLIBC_PREFIX}gcc ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	${ARM_GLIBC_PREFIX}strip $@

${MQTTLIB_ARM_UCLIBC}: ${SOURCE_FILES} ${HEADERS}
	-mkdir linux_ARM_uclibc
	${ARM_UCLIBC_PREFIX}gcc ${CCFLAGS_SO} ${LDFLAGS} -o $@ ${SOURCE_FILES}
	${ARM_UCLIBC_PREFIX}strip $@
endif

endif

endif


ifeq ($(OSTYPE),AIX)
# applications must be linked with -I. -L. -lmqttv3c -Wl,-brtl -pthread
  CC 	   = gcc

  CCFLAGS_SO = -fPIC -Os -Wall -shared -DREVERSED
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

  # linker flags
  LINKFLAGS = /nologo /machine:x86 /manifest 
  WINLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib
  # Flags to link C libraries
  LIBLINKFLAGS = /dll   
  IMP = /implib:$(@:.dll=.lib)
  LIBPDB = /pdb:$(@:.dll=.pdb)
  LIBMAP = /map:$(@:.dll=.map)
  LIBLDFLAGS = $(LINKFLAGS) $(WINLIBS) $(LIBLINKFLAGS) $(IMP) $(LIBPDB) $(LIBMAP)
  # Flags to link C programs
  MAINLINKFLAGS = /subsystem:console
  EXEPDB = /pdb:$(@:.exe=.pdb)
  EXEMAP = /map:$(@:.exe=.map)
  MAINLDFLAGS = $(LINKFLAGS) $(WINLIBS) $(MAINLINKFLAGS) $(EXEPDB) $(EXEMAP) 

  MQTTLDLIB = windows_ia32/$(MQTTLIB).lib
  MQTTDLL   = windows_ia32/$(MQTTLIB).dll

all: ${MQTTDLL}

$(MQTTDLL): $(shell cygpath -au ${SOURCE_FILES})  $(shell cygpath -au ${HEADERS})
	-mkdir windows_ia32
	${CC} ${CPPFLAGS} ${CFLAGS} /I $(shell cygpath -am ${MQTTCLIENT_DIR}) /I $(shell cygpath -am ${MQTTCLIENT_DIR})/.. /Fo $(shell cygpath -am ${SOURCE_FILES})
	$(LD) $(LIBLDFLAGS) *.obj /out:$(MQTTDLL)
	mt -manifest windows_ia32/mqttv3c.dll.manifest -outputresource:$(MQTTDLL)\;2

endif  

