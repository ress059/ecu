#------------------------------------------------------------#
#---------------- INITIAL PROJECT INVOKATION ----------------#
#------------------------------------------------------------#
cmake_minimum_required(VERSION 3.21)
project(ECU_LIBRARY VERSION 0.1)
set(ECU_SUPPORTED_COMPILERS "GNU")
option(ECU_DISABLE_RUNTIME_ASSERTS OFF)

if(NOT CMAKE_C_COMPILER_ID IN_LIST ECU_SUPPORTED_COMPILERS)
    message(WARNING "Using untested compiler. Currently supported compilers = ${ECU_SUPPORTED_COMPILERS}")
endif()

#------------------------------------------------------------#
#----------------------- ECU SETTINGS -----------------------#
#------------------------------------------------------------#
# Note this library is meant to be compiled with the target 
# application's toolchain.
add_library(ecu STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/asserter.c
    ${CMAKE_CURRENT_LIST_DIR}/src/dlist.c
    ${CMAKE_CURRENT_LIST_DIR}/src/event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/fsm.c
    ${CMAKE_CURRENT_LIST_DIR}/src/hsm.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ntnode.c
    ${CMAKE_CURRENT_LIST_DIR}/src/object_id.c
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.c
)

# Example include in the Application would be #include "ecu/fsm.h" 
# in order to give larger apps some scope when including files.
target_include_directories(ecu
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/inc 
)

# ECU library requires at least C99.
target_compile_features(ecu 
    PUBLIC 
        c_std_99    
)

# Target Endianness.
if(CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
    target_compile_definitions(ecu 
        PUBLIC 
            ECU_LITTLE_ENDIAN 
    )
elseif(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
    target_compile_definitions(ecu 
        PUBLIC 
            ECU_BIG_ENDIAN 
    )
else()
    message(WARNING "Unable to detect target endianness. endian.h cannot be used.")
endif() 

# Store ECU-specific warning flags in an interface target in case it is
# also needed by test builds. Enable all warnings and relevant debug info.
# An optimization flag is not specified on purpose so it can be defined by 
# the application. I.e. after importing ecu the application can do: 
# target_compile_options(ecu PRIVATE -O2). It is OK if the application does 
# not specify an optimization level - GCC will default it to -O0.
# Note that -Werror will only be passed for build tests.
add_library(ecu_warning_flags INTERFACE)
target_compile_options(ecu_warning_flags 
    INTERFACE 
        # C compiler flags. We know ECU is a C library so we can use this generator expression.
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always -ffunction-sections -fdata-sections -fstack-usage -fno-common>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wshadow -Wstack-usage=200>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wcast-align=strict -Wstrict-prototypes -Wstrict-overflow=2 -Wwrite-strings -Waggregate-return>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wcast-qual -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion>

        # Compiler flags specific to Debug builds
        $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-g3>
)

# Warn the user that run-time asserts are being disabled. It is
# not recommended to disable run-time asserts but we have this
# as an option to decrease overhead of the library.
if(ECU_DISABLE_RUNTIME_ASSERTS)
    message(WARNING "Option ECU_DISABLE_RUNTIME_ASSERTS set to ON. Runtime asserts "
                    "will not fire in this library. Are you sure you wish to disable asserts??")

    target_compile_definitions(ecu 
        PUBLIC 
            ECU_DISABLE_RUNTIME_ASSERTS 
    )

    # Some functions and parameters will now be unused because they are 
    # only called within assert macros. Suppress these warnings.
    target_compile_options(ecu_warning_flags
        INTERFACE
            $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-unused-function -Wno-unused-parameter>
    )
endif()

# Note how ECU-specific warning flags are private so it is not propagated
# to the user's application. However our internal tests can still use 
# ecu_warning_flags target to get all of the warning flags.
target_link_libraries(ecu 
    PRIVATE
        ecu_warning_flags
)

#------------------------------------------------------------#
#--------- CREATE --target doxygen and sphinx RULES ---------#
#------------------------------------------------------------#
# Rule requires doxygen, doxysphinx, and sphinx. 
# - sudo apt-get install doxygen
# - pip install doxysphinx
# - pip install sphinx_rtd_theme
# - pip install sphinxcontrib-doxylink
# - pip install sphinx-tabs
if(ECU_ENABLE_BUILD_TESTING OR ECU_ENABLE_UNIT_TESTING)
    # Adapted from this source: https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/inc/ecu)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/doc/doxygen)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYGEN_TOP_LEVEL_DIR ${CMAKE_CURRENT_LIST_DIR}) # STRIP_FROM_PATH in Doxyfile.
    set(DOXYGEN_WARNING_LOG_OUT ${CMAKE_CURRENT_LIST_DIR}/doc/build/doxygen_warning_log.txt)
    set(DOXYFILE_IN ${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_LIST_DIR}/Doxyfile) # Put in top-level directory for readthedocs CI steps.
    file(GLOB_RECURSE ECU_PUBLIC_HEADERS 
        CONFIGURE_DEPENDS # Also run if file added or removed.
            ${DOXYGEN_INPUT_DIR}/*.h
    )
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    # Only regenerate Doxygen when the Doxyfile or public headers change
    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${ECU_PUBLIC_HEADERS}
        COMMAND ${CMAKE_COMMAND} -E echo "Generating doxygen documentation ${DOXYGEN_INDEX_FILE}..."
        COMMAND doxygen ${DOXYFILE_OUT}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        VERBATIM
    )

    # Create phony target that always generates doxygen documentation.
    add_custom_target(doxygen 
        COMMAND ${CMAKE_COMMAND} -E echo "Generating doxygen documentation ${DOXYGEN_INDEX_FILE}..."
        COMMAND doxygen ${DOXYFILE_OUT}
    )

    set(SPHINX_INPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/doc)
    set(SPHINX_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/doc/build)
    set(SPHINX_INDEX_FILE ${SPHINX_OUTPUT_DIR}/index.html)
    file(GLOB_RECURSE ECU_SPHINX_FILES 
        # Do not use CONFIGURE_DEPENDS since it makes the target rule slow (many files have to be searched).
        ${SPHINX_INPUT_DIR}/*.rst
        ${SPHINX_INPUT_DIR}/images/*.svg
    )
    
    # Only regenerate Sphinx when:
    # - Doxygen has rerun
    # - Our doc files have been updated
    # - The Sphinx config has been updated
    add_custom_command(
        OUTPUT ${SPHINX_INDEX_FILE}
        DEPENDS ${DOXYGEN_INDEX_FILE} ${ECU_SPHINX_FILES}
        MAIN_DEPENDENCY ${SPHINX_INPUT_DIR}/conf.py
        COMMAND ${CMAKE_COMMAND} -E echo "Generating sphinx documentation ${SPHINX_INDEX_FILE}..."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SPHINX_OUTPUT_DIR} # In case it does not already exist
        COMMAND doxysphinx build ${SPHINX_INPUT_DIR} ${SPHINX_OUTPUT_DIR} ${DOXYGEN_OUTPUT_DIR}/html
        COMMAND sphinx-build -b html ${SPHINX_INPUT_DIR} ${SPHINX_OUTPUT_DIR}
        VERBATIM
    )

    add_custom_target(sphinx 
        DEPENDS ${SPHINX_INDEX_FILE}
    )
endif()

#------------------------------------------------------------#
#---------------------- INTERNAL TESTS ----------------------#
#------------------------------------------------------------#
if(ECU_ENABLE_UNIT_TESTING)
    target_compile_options(ecu
        PRIVATE
            # Compile for code coverage for ECU source code.
            $<$<COMPILE_LANG_AND_ID:C,GNU>:-O0 --coverage -fprofile-abs-path>
    )

    target_compile_definitions(ecu
        PUBLIC 
            ECU_UNIT_TEST 
    )

    # CTest and enable_testing has to be in the root CMakeLists.txt. 
    # Tests will not be discovered if this is in tests/CMakeLists.txt....
    include(CTest)
    enable_testing()
    add_subdirectory(tests/unit)
elseif(ECU_ENABLE_BUILD_TESTING)
    add_subdirectory(tests/build)
endif()
