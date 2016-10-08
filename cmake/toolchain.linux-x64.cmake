# Name of the target platform
set(CMAKE_SYSTEM_NAME Linux)

# Version of the system
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-undefined" CACHE STRING "" FORCE)

# compiler flags
set(CMAKE_C_FLAGS "--std=gnu99 -Werror=implicit-function-declaration" CACHE STRING "" FORCE)

# paho specific settings
set(PAHO_LIB_PREFIX "lib")
set(PAHO_PLATFORM "linux-x64")
