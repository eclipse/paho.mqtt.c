# Name of the target platform
set(CMAKE_SYSTEM_NAME Windows)

# Version of the system
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
set(CMAKE_RC_COMPILER_ENV_VAR "RC")
set(CMAKE_RC_COMPILER "")
set(CMAKE_C_FLAGS "--std=gnu99 -Werror=implicit-function-declaration")
set(CMAKE_SHARED_LINKER_FLAGS
    "-fdata-sections -ffunction-sections -Wl,--enable-stdcall-fixup -static-libgcc -static -lpthread" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS
    "-fdata-sections -ffunction-sections -Wl,--enable-stdcall-fixup -static-libgcc -static -lpthread" CACHE STRING "" FORCE)

# paho specific settings
set(PAHO_LIB_PREFIX "")
set(PAHO_PLATFORM "win32")
