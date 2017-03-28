#*******************************************************************************
#  Copyright (c) 2015, 2017 logi.cals GmbH and others
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
#     Rainer Poisel - initial version
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

PROJECT("paho" C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "CMake version: " ${CMAKE_VERSION})
MESSAGE(STATUS "CMake system name: " ${CMAKE_SYSTEM_NAME})

## build settings
SET(PAHO_VERSION_MAJOR 1)
SET(PAHO_VERSION_MINOR 1)
SET(PAHO_VERSION_PATCH 0)
SET(CLIENT_VERSION ${PAHO_VERSION_MAJOR}.${PAHO_VERSION_MINOR}.${PAHO_VERSION_PATCH})


EXECUTE_PROCESS(COMMAND date -u OUTPUT_VARIABLE BUILD_TIMESTAMP)

IF (BUILD_TIMESTAMP STREQUAL "")
	MESSAGE(WARNING "Unable to determine build timestamp via default mechanism.")

	IF (WIN32)
		EXECUTE_PROCESS(COMMAND "cmd.exe" "/c" echo %DATE% %TIME% OUTPUT_VARIABLE BUILD_TIMESTAMP)
	ENDIF()
ENDIF()

STRING(STRIP ${BUILD_TIMESTAMP} BUILD_TIMESTAMP)
MESSAGE(STATUS "Timestamp is ${BUILD_TIMESTAMP}")


# Uses the correct directory for libraries on Red Hat-based distributions.
SET(PAHO_LIBRARY_DIR_NAME "lib")
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  IF(EXISTS "/etc/redhat-release")
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    	SET(PAHO_LIBRARY_DIR_NAME "lib64")
    ENDIF()
  ENDIF()
ENDIF()

# Uses the correct directory for libraries on Red Hat-based distributions.
SET(PAHO_LIBRARY_DIR_NAME "lib")
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  IF(EXISTS "/etc/redhat-release")
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    	SET(PAHO_LIBRARY_DIR_NAME "lib64")
    ENDIF()
  ENDIF()
ENDIF()

IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -DWIN32_LEAN_AND_MEAN -MD)
ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  ADD_DEFINITIONS(-DOSX)
ENDIF()

## build options
SET(PAHO_WITH_SSL FALSE CACHE BOOL "Flag that defines whether to build ssl-enabled binaries too. ")
SET(PAHO_BUILD_STATIC FALSE CACHE BOOL "Build static library")
SET(PAHO_BUILD_DOCUMENTATION FALSE CACHE BOOL "Create and install the HTML based API documentation (requires Doxygen)")
SET(PAHO_BUILD_SAMPLES FALSE CACHE BOOL "Build sample programs")

ADD_SUBDIRECTORY(src)
IF(PAHO_BUILD_SAMPLES)
    ADD_SUBDIRECTORY(src/samples)
ENDIF()

IF(PAHO_BUILD_DOCUMENTATION)
    ADD_SUBDIRECTORY(doc)
ENDIF()

### packaging settings
IF (WIN32)
    SET(CPACK_GENERATOR "ZIP")
ELSEIF(UNIX)
    SET(CPACK_GENERATOR "TGZ")
ENDIF()

SET(CPACK_PACKAGE_VERSION_MAJOR ${PAHO_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PAHO_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PAHO_VERSION_PATCH})
INCLUDE(CPack)

ENABLE_TESTING()

INCLUDE_DIRECTORIES(test src)
ADD_SUBDIRECTORY(test)
