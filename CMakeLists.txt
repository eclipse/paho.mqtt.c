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

project("paho" C)
cmake_minimum_required(VERSION 3.0)

if (NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR
        "Paho has to be built using a toolchain file. \
These can be found in ${CMAKE_CURRENT_LIST_DIR}/cmake and can be \
specified using the CMAKE_TOOLCHAIN_FILE variable when invoking cmake."
    )
endif ()

## build settings
file(READ version.major PAHO_VERSION_MAJOR)
file(READ version.minor PAHO_VERSION_MINOR)
file(READ version.patch PAHO_VERSION_PATCH)
set(CLIENT_VERSION ${PAHO_VERSION_MAJOR}.${PAHO_VERSION_MINOR}.${PAHO_VERSION_PATCH})

string(TIMESTAMP BUILD_TIMESTAMP UTC)

## build options
set(PAHO_WITH_SSL FALSE CACHE BOOL "Flag that defines whether to build ssl-enabled binaries too. ")
set(PAHO_BUILD_DOCUMENTATION FALSE CACHE BOOL "Create and install the HTML based API documentation (requires Doxygen)")
set(PAHO_BUILD_SAMPLES FALSE CACHE BOOL "Build sample programs")

add_subdirectory(src)
if(PAHO_BUILD_SAMPLES)
    add_subdirectory(src/samples)
endif()

if(PAHO_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

### packaging settings
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

set(CPACK_PACKAGE_FILE_NAME paho-${PAHO_VERSION_MAJOR}.${PAHO_VERSION_MINOR}.${PAHO_VERSION_PATCH}-${PAHO_PLATFORM})
include(CPack)
