#*******************************************************************************
#  Copyright (c) 2015 logi.cals GmbH
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
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

## build settings
SET(PAHO_VERSION_MAJOR 1)
SET(PAHO_VERSION_MINOR 0)
SET(PAHO_VERSION_PATCH 3)
SET(CLIENT_VERSION ${PAHO_VERSION_MAJOR}.${PAHO_VERSION_MINOR}.${PAHO_VERSION_PATCH})

EXECUTE_PROCESS(COMMAND date -u OUTPUT_VARIABLE BUILD_TIMESTAMP)
STRING(STRIP ${BUILD_TIMESTAMP} BUILD_TIMESTAMP)

## build options
SET(PAHO_WITH_SSL FALSE CACHE BOOL "Flag that defines whether to build ssl-enabled binaries too. ")
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
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET(CPACK_GENERATOR "ZIP")
ELSE()
    SET(CPACK_GENERATOR "TGZ")
ENDIF()

SET(CPACK_PACKAGE_VERSION_MAJOR ${PAHO_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PAHO_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PAHO_VERSION_PATCH})
INCLUDE(CPack)
