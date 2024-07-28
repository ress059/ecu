# This toolchain file is for compiling and running on a Linux x86 machine using Linux x86 GNU toolchain.
# A Linux machine is required for this toolchain file. Used this resource as a reference:
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html
# Target = Linux x86 machine.
# Host = Linux x86 machine.
message(STATUS "Using toolchain file for compiling and running on Linux x86 machine using Linux x86 GNU toolchain. " 
               "Must be using Linux x86 machine with linux x86 GNU toolchain installed in order to use this toolchain file.")


# Required.
# When the compiler is first detected CMake by default attempts to compile a test executable.
# We can't do this since we are also setting the linker here. Have CMake only compile a static
# library for its test to avoid using the linker.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY" CACHE INTERNAL "")


# Give CMake some hints on where to search for GNU toolchain. Use this variable instead of 
# GNUInstallDirs to suppress its warnings since compiler path not yet set. Note that 
# CMAKE_SYSTEM_NAME, CMAKE_SYSTEM_PROCESSOR, CMAKE_FIND_ROOT_PATH, CMAKE_FIND_ROOT_PATH_MODE_PROGRAM,
# CMAKE_FIND_ROOT_PATH_MODE_LIBRARY, and CMAKE_FIND_ROOT_PATH_MODE_INCLUDE are not specified
# since we are not cross compiling.
set(TOOLCHAIN_PATH_HINTS "usr/bin" "usr/local/bin" CACHE INTERNAL "")


if(CMAKE_HOST_LINUX OR CMAKE_HOST_UNIX)
    find_program(CMAKE_C_COMPILER		NAMES gcc			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED)
    find_program(CMAKE_CXX_COMPILER		NAMES g++			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED)
    find_program(LINKER					NAMES ld			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED)
    find_program(CMAKE_AR				NAMES gcc-ar		HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Creates, modifies, and extracts from .a libraries
    find_program(CMAKE_RANLIB			NAMES gcc-ranlib	HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Generate indices for .a libraries
    find_program(ASSEMBLER				NAMES as			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED)
    find_program(GPROF					NAMES gprof			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Monitor program performance
    find_program(NM						NAMES gcc-nm		HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # List symbols exported by .o file. Measure code size in more detail (symbols).
    find_program(OBJCOPY				NAMES objcopy		HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Copy .o file
    find_program(OBJDUMP				NAMES objdump		HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Display info about .o files
    find_program(SIZE					NAMES size			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Measure overall code size (sections)
    find_program(STRIP					NAMES strip			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Discard symbols from .o files
    find_program(GCOV					NAMES gcov			HINTS ${TOOLCHAIN_PATH_HINTS} REQUIRED) # Code coverage
else()
    message(FATAL_ERROR "Only Linux hosts currently supported.")
endif()


# Do not let CMake automatically add some compiler flags based on the build type.
# In this case we want full control over the flags we pass to the compiler.
set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "")
set(CMAKE_C_FLAGS_MINSIZEREL "-DNDEBUG" CACHE STRING "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-DNDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-DNDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG" CACHE STRING "")
