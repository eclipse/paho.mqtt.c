# path to compiler and utilities
# specify the cross compiler
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_C_FLAGS "--std=gnu99 -Werror=implicit-function-declaration")

# Name of the target platform
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Version of the system
set(CMAKE_SYSTEM_VERSION 1)

# paho specific settings
set(PAHO_LIB_PREFIX "lib")
set(PAHO_PLATFORM "linux-arm11")
