# This toolchain file is for compiling Linux application on Linux host.
# Target = Linux x86_64 machine.
# Host = Linux x86_64 machine.
message(STATUS "Using toolchain file for compiling on Linux x86_64 host.")

if(CMAKE_HOST_LINUX OR CMAKE_HOST_UNIX)
    find_program(CMAKE_C_COMPILER		    NAMES gcc			REQUIRED)
    find_program(CMAKE_CXX_COMPILER		    NAMES g++			REQUIRED)
    find_program(LINKER                     NAMES ld		    REQUIRED)
    find_program(CMAKE_AR				    NAMES gcc-ar		REQUIRED) # Creates, modifies, and extracts from .a libraries
    find_program(CMAKE_RANLIB			    NAMES gcc-ranlib	REQUIRED) # Generate indices for .a libraries
    find_program(ASSEMBLER				    NAMES as			REQUIRED)
    find_program(GPROF					    NAMES gprof			REQUIRED) # Monitor program performance
    find_program(NM						    NAMES gcc-nm		REQUIRED) # List symbols exported by .o file. Measure code size in more detail (symbols).
    find_program(OBJCOPY				    NAMES objcopy		REQUIRED) # Copy .o file
    find_program(OBJDUMP				    NAMES objdump		REQUIRED) # Display info about .o files
    find_program(SIZE					    NAMES size			REQUIRED) # Measure overall code size (sections)
    find_program(STRIP					    NAMES strip			REQUIRED) # Discard symbols from .o files
    find_program(GCOV					    NAMES gcov			REQUIRED) # Code coverage
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
