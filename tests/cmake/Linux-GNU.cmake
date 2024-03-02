# Required.
# When the compiler is first detected CMake by default attempts to compile a test executable.
# We can't do this since we are also setting the linker here. Have CMake only compile a static
# library for its test to avoid using the linker.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# !!!!! TODO. Write python scrip to check gcc and g++ versions?? !!!!!!!!
if(CMAKE_HOST_LINUX OR CMAKE_HOST_UNIX)
    include(GNUInstallDirs)
    message(STATUS "Detected host system is Linux. Searching default install"
				 	"directories usr/bin and usr/local/bin for GNU toolchain..")

	find_program(LINKER              	NAMES ld        	HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED)
	find_program(CMAKE_C_COMPILER		NAMES gcc			HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED)
	find_program(CMAKE_CXX_COMPILER 	NAMES g++			HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED)
	find_program(CMAKE_AR            	NAMES gcc-ar		HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Creates, modifies, and extracts from .a libraries
	find_program(CMAKE_RANLIB        	NAMES gcc-ranlib    HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Generate indices for .a libraries

	find_program(ASSEMBLER        		NAMES as        	HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED)
	
	find_program(GPROF					NAMES gprof 		HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Monitor program performance
	find_program(NM                  	NAMES gcc-nm        HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # List symbols exported by .o file. Measure code size in more detail (symbols).
	find_program(OBJCOPY             	NAMES objcopy   	HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Copy .o file
	find_program(OBJDUMP				NAMES objdump 		HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Display info about .o files
	find_program(SIZE                	NAMES size      	HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Measure overall code size (sections)
	find_program(STRIP               	NAMES strip     	HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Discard symbols from .o files
	find_program(GCOV                	NAMES gcov      	HINTS ${CMAKE_INSTALL_BINDIR} REQUIRED) # Code coverage

else()
    message(FATAL_ERROR "Only Linux hosts currently supported.")
endif()
###########################################################################################################################

# Linker
# set(CMAKE_EXE_LINKER_FLAGS_INIT		"${CMAKE_EXE_LINKER_FLAGS_INIT} ${LINKER}") 	# Linker command when linking to create executables.
# set(CMAKE_MODULE_LINKER_FLAGS_INIT	"${CMAKE_MODULE_LINKER_FLAGS_INIT} ${LINKER}") 	# Linker command when linking to static libraries.
# set(CMAKE_SHARED_LINKER_FLAGS_INIT	"${CMAKE_SHARED_LINKER_FLAGS_INIT} ${LINKER}") 	# Linker command whne linking to dynamic libraries.
###########################################################################################################################

# Compiler flags. Still deciding on -Werror
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -fdiagnostics-color=always")
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wcast-align")
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -Wstrict-prototypes") # All compiler flags are the same for C and C++ besides this one.
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -Wstrict-overflow=2 -Wwrite-strings -Waggregate-return -Wcast-qual")
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion")

set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wcast-align")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -Wstrict-overflow=2 -Wwrite-strings -Waggregate-return -Wcast-qual")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion")
###########################################################################################################################