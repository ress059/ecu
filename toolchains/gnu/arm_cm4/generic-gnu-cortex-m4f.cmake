# This toolchain file is for cross compiling ARM Cortex M4F target on an x86_64 Linux 
# host with arm-none-eabi toolchain:
# https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
# "x86_64 Linux hosted cross toolchains: AArch32 bare-metal target (arm-none-eabi)" Section.
# generic-gnu-cortex-m4f.cmake is for Cortex-M4 targets WITH a floating point unit. 
# generic-gnu-cortex-m4.cmake is for Cortex-M4 targets WITHOUT a floating point unit.
# 
# If creating an executable, application build system is responsible for 
# microcontroller-specific linker script, startup code, compiler flags, and linker flags. 
# This file is meant to be generic to all Cortex M4F MCUs, so it is not appropriate to 
# define these MCU-specific properties here.
#
# Used this resource as a reference:
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html
# Target = ARM Cortex-M4F MCU.
# Host = Linux x86_64 machine.
message(STATUS "Using toolchain file for cross compiling ARM Cortex-M4F target on Linux x86_64 host.")

set(CMAKE_SYSTEM_NAME Generic CACHE INTERNAL "")
set(CMAKE_CROSSCOMPILING TRUE CACHE INTERNAL "")
set(CMAKE_SYSTEM_PROCESSOR cortex-m4 CACHE STRING "") # Older versions of CMake use this to append to -mcpu= flag.

# Prevent CMake from performing internal compilation and linking tests since we are cross compiling
set(CMAKE_C_COMPILER_WORKS TRUE CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS TRUE CACHE INTERNAL "")

# Find arm-none-eabi. For GNU, the only ones that matter for building are CMAKE_C_COMPILER
# and CMAKE_CXX_COMPILER since gcc/g++ will automatically invoke the linker, assembler, etc.
# We still do this to ensure the proper linker, assembler, etc actually exists.
if(CMAKE_HOST_LINUX OR CMAKE_HOST_UNIX)
    find_program(CMAKE_C_COMPILER		NAMES arm-none-eabi-gcc			REQUIRED)
    find_program(CMAKE_CXX_COMPILER		NAMES arm-none-eabi-g++			REQUIRED)
    find_program(LINKER					NAMES arm-none-eabi-ld			REQUIRED)
    find_program(CMAKE_AR				NAMES arm-none-eabi-gcc-ar		REQUIRED) # Creates, modifies, and extracts from .a libraries
    find_program(CMAKE_RANLIB			NAMES arm-none-eabi-gcc-ranlib	REQUIRED) # Generate indices for .a libraries
    find_program(ASSEMBLER				NAMES arm-none-eabi-as			REQUIRED)
    find_program(GPROF					NAMES arm-none-eabi-gprof		REQUIRED) # Monitor program performance
    find_program(NM						NAMES arm-none-eabi-gcc-nm		REQUIRED) # List symbols exported by .o file. Measure code size in more detail (symbols).
    find_program(OBJCOPY				NAMES arm-none-eabi-objcopy		REQUIRED) # Copy .o file
    find_program(OBJDUMP				NAMES arm-none-eabi-objdump		REQUIRED) # Display info about .o files
    find_program(SIZE					NAMES arm-none-eabi-size		REQUIRED) # Measure overall code size (sections)
    find_program(STRIP					NAMES arm-none-eabi-strip		REQUIRED) # Discard symbols from .o files
    find_program(GCOV					NAMES arm-none-eabi-gcov		REQUIRED) # Code coverage
else()
    message(FATAL_ERROR "Only Linux hosts currently supported.")
endif()

# Do not let CMake automatically add compiler flags based on the build configuration. 
# I.e. for Release builds CMake automatically adds -O3. We do not want this - we want
# full control over the compiler flags we pass in so we ensure these CMake variables 
# that control this behavior are empty.
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE STRING "")
set(CMAKE_ASM_FLAGS_MINSIZEREL "-DNDEBUG" CACHE STRING "")
set(CMAKE_ASM_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "")
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "-DNDEBUG" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "")
set(CMAKE_C_FLAGS_MINSIZEREL "-DNDEBUG" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-DNDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-DNDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG" CACHE STRING "")

# Set hardware-specific compiler flags for ARM CM4F, stdlib implementation to use, and
# linker garbage collection flags here since these should apply to the entire project.
# Remaining flags will be added by CMake build system depending on what is needed 
# by the application.
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -ffunction-sections -fdata-sections" CACHE STRING "")
set(CMAKE_CXX_FLAGS_INIT "-fno-exceptions -fno-rtti -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -ffunction-sections -fdata-sections" CACHE STRING "")

# Set hardware-specific linker flags for ARM CM4F, stdlib implementation to use, and
# linker garbage collection flags here since these should apply to the entire project.
# Remaining flags will be added by CMake build system depending on what is needed 
# by the application.
set(CMAKE_EXE_LINKER_FLAGS_INIT "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -specs=nano.specs -Wl,--gc-sections" CACHE STRING "")

# These rule variables control how CMake actually calls the linker from the command line.
# Edit these so it creates an .elf executable. All other flags remain the same as the default.
# Verified linker invokation is exactly the same besides the .elf
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET>.elf <LINK_LIBRARIES>" CACHE STRING "")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET>.elf <LINK_LIBRARIES>" CACHE STRING "")
