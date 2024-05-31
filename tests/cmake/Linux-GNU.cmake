# Required.
# When the compiler is first detected CMake by default attempts to compile a test executable.
# We can't do this since we are also setting the linker here. Have CMake only compile a static
# library for its test to avoid using the linker.
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

if(CMAKE_HOST_LINUX OR CMAKE_HOST_UNIX)
	# Use list instead of GNUInstallDirs to suppress its warnings since compiler path not yet set.
	list(APPEND PATH_HINTS "usr/bin" "usr/local/bin")
	message(STATUS "Detected host system is Linux. Searching default install"
				   " directories usr/bin and usr/local/bin for GNU toolchain..")

	find_program(LINKER					NAMES ld			HINTS ${PATH_HINTS} REQUIRED)
	find_program(CMAKE_C_COMPILER		NAMES gcc			HINTS ${PATH_HINTS} REQUIRED)
	find_program(CMAKE_CXX_COMPILER		NAMES g++			HINTS ${PATH_HINTS} REQUIRED)
	find_program(CMAKE_AR				NAMES gcc-ar		HINTS ${PATH_HINTS} REQUIRED) # Creates, modifies, and extracts from .a libraries
	find_program(CMAKE_RANLIB			NAMES gcc-ranlib	HINTS ${PATH_HINTS} REQUIRED) # Generate indices for .a libraries
	find_program(ASSEMBLER				NAMES as			HINTS ${PATH_HINTS} REQUIRED)
	find_program(GPROF					NAMES gprof			HINTS ${PATH_HINTS} REQUIRED) # Monitor program performance
	find_program(NM						NAMES gcc-nm		HINTS ${PATH_HINTS} REQUIRED) # List symbols exported by .o file. Measure code size in more detail (symbols).
	find_program(OBJCOPY				NAMES objcopy		HINTS ${PATH_HINTS} REQUIRED) # Copy .o file
	find_program(OBJDUMP				NAMES objdump		HINTS ${PATH_HINTS} REQUIRED) # Display info about .o files
	find_program(SIZE					NAMES size			HINTS ${PATH_HINTS} REQUIRED) # Measure overall code size (sections)
	find_program(STRIP					NAMES strip			HINTS ${PATH_HINTS} REQUIRED) # Discard symbols from .o files
	find_program(GCOV					NAMES gcov			HINTS ${PATH_HINTS} REQUIRED) # Code coverage
else()
    message(FATAL_ERROR "Only Linux hosts currently supported.")
endif()
###########################################################################################################################

# Linker
# set(CMAKE_EXE_LINKER_FLAGS_INIT		"${CMAKE_EXE_LINKER_FLAGS_INIT} ${LINKER}")		# Linker command when linking to create executables.
# set(CMAKE_MODULE_LINKER_FLAGS_INIT	"${CMAKE_MODULE_LINKER_FLAGS_INIT} ${LINKER}")	# Linker command when linking to static libraries.
# set(CMAKE_SHARED_LINKER_FLAGS_INIT	"${CMAKE_SHARED_LINKER_FLAGS_INIT} ${LINKER}")	# Linker command whne linking to dynamic libraries.
###########################################################################################################################
