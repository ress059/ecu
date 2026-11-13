#------------------------------------------------------------#
#---------------- INITIAL PROJECT INVOKATION ----------------#
#------------------------------------------------------------#
cmake_minimum_required(VERSION 3.28)
project(ECU_LIBRARY VERSION 0.1)
set(ECU_SUPPORTED_COMPILERS "GNU")
option(ECU_DISABLE_ASSERTS OFF)

if(NOT CMAKE_C_COMPILER_ID IN_LIST ECU_SUPPORTED_COMPILERS)
    message(WARNING "Using untested compiler. Currently supported compilers = ${ECU_SUPPORTED_COMPILERS}")
endif()

#------------------------------------------------------------#
#------------------------ ecu target ------------------------#
#------------------------------------------------------------#
# Main target used by the application.
add_library(ecu STATIC EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_LIST_DIR}/src/asserter.c
    ${CMAKE_CURRENT_LIST_DIR}/src/dlist.c
    ${CMAKE_CURRENT_LIST_DIR}/src/event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/fsm.c
    ${CMAKE_CURRENT_LIST_DIR}/src/hsm.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ntnode.c
    ${CMAKE_CURRENT_LIST_DIR}/src/object_id.c
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.c
)

target_include_directories(ecu
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/inc # Gives include directives scope. I.e. #include "ecu/fsm.h" instead of #include "fsm.h"
)

target_compile_features(ecu 
    PUBLIC 
        c_std_99 # ECU requires at least C99.
)

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

# Warn the user that run-time asserts are being disabled. It is
# not recommended to disable run-time asserts but we have this
# as an option to decrease overhead of the library.
if(ECU_DISABLE_ASSERTS)
    message(WARNING "Option ECU_DISABLE_ASSERTS set to ON. ECU runtime asserts disabled.")

    target_compile_definitions(ecu 
        PUBLIC 
            ECU_DISABLE_ASSERTS 
    )

    # Some functions and parameters will now be unused because they are 
    # only called within assert macros. Suppress these warnings.
    target_compile_options(ecu
        PRIVATE
            $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-unused-function -Wno-unused-parameter>
    )
endif()

#------------------------------------------------------------#
#--------------------- INTERNAL TARGETS ---------------------#
#------------------------------------------------------------#
if(ECU_INTERNAL)
    get_target_property(ECU_SOURCES ecu SOURCES)
    file(GLOB_RECURSE ECU_INCLUDES 
        CONFIGURE_DEPENDS # Also run if file added or removed.
            ${CMAKE_CURRENT_LIST_DIR}/inc/*.h
    )

    #------------------------------------------------------------#
    #--------------------- ecu_common target --------------------#
    #------------------------------------------------------------#
    # Target containing common compiler flags, settings, etc.
    # Allows library to be easily rebuilt with different preprocessor defines, compiler options, etc.
    # Note that sources are not included in this target to allow common compiler flags to be applied to targets
    # that do not need to recompile ECU. 
    add_library(ecu_common INTERFACE)

    target_compile_options(ecu_common
        INTERFACE
            # C and C++ flags
            $<$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>:-fdiagnostics-color=always -ffunction-sections -fdata-sections>
            $<$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>:-Wall -Werror -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wshadow>
            $<$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>:-Wcast-align=strict -Wstrict-overflow=2 -Wwrite-strings>
            $<$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>:-Wcast-qual -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion>

            # C-only flags
            $<$<COMPILE_LANG_AND_ID:C,GNU>:-fno-common -Wstrict-prototypes -Waggregate-return>

            # Debug settings
            $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-g3>
    )

    target_include_directories(ecu_common
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/inc
    )

    if(CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
        target_compile_definitions(ecu_common 
            INTERFACE 
                ECU_LITTLE_ENDIAN 
        )
    elseif(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
        target_compile_definitions(ecu_common 
            INTERFACE 
                ECU_BIG_ENDIAN 
        )
    else()
        # Notice this is a fatal error instead of a warning since this target is only used internally.
        message(FATAL_ERROR "Unable to detect target endianness. endian.h cannot be used.")
    endif() 

    if(ECU_DISABLE_ASSERTS)
        target_compile_definitions(ecu_common
            INTERFACE 
                ECU_DISABLE_ASSERTS 
        )
    endif()

    #------------------------------------------------------------#
    #------------------- clang format targets -------------------#
    #------------------------------------------------------------#
    # Phony target that clang-formats ECU source code in place. Meant to be used by developer.
    add_custom_target(clang_format
        COMMENT "Formatting all ECU files: ${ECU_INCLUDES} ${ECU_SOURCES}"
        COMMAND clang-format --Werror --fallback-style=none --ferror-limit=0 --style=file -i ${ECU_INCLUDES} ${ECU_SOURCES}
    )

    # Phony target that returns an error code if ECU source code is not clang-formatted. This is used by CI to fail a job if
    # ECU changes were pushed but not formatted. Files are NOT edited to force the developer to look over and format the files.
    add_custom_target(clang_format_ci # Returns error code if formatting error but does not format any files.
        COMMENT "Checking for formatting errors in all ECU files: ${ECU_INCLUDES} ${ECU_SOURCES}"
        COMMAND clang-format --Werror --fallback-style=none --ferror-limit=0 --style=file --dry-run ${ECU_INCLUDES} ${ECU_SOURCES}
    )

    #------------------------------------------------------------#
    #---------------------- doxygen target ----------------------#
    #------------------------------------------------------------#
    # Target that generates doxygen documentation locally if:
    #   - Any ECU public headers change.
    #   - Doxyfile.in changes.
    set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_LIST_DIR}/doc/doxygen/html/index.html)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in ${CMAKE_CURRENT_LIST_DIR}/Doxyfile @ONLY)

    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${ECU_INCLUDES} ${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/doc/doxygen
        COMMAND doxygen ${CMAKE_CURRENT_LIST_DIR}/Doxyfile
        COMMAND ${CMAKE_COMMAND} -E echo "Generated doxygen documentation in: ${DOXYGEN_INDEX_FILE}"
        VERBATIM
    )

    add_custom_target(doxygen 
        DEPENDS ${DOXYGEN_INDEX_FILE}
    )

    #------------------------------------------------------------#
    #----------------------- sphinx target ----------------------#
    #------------------------------------------------------------#
    # Target that generates sphinx documentation locally. Only runs if:
    #   - Doxygen needs to be rerun (ECU public headers changed or Doxyfile.in changed).
    #   - Sphinx files changed (.rst or images input to Sphinx)
    #   - Sphinx conf.py has changed.
    file(GLOB_RECURSE SPHINX_FILES 
        # Do not use CONFIGURE_DEPENDS since it makes the target rule slow (many files have to be searched).
        ${CMAKE_CURRENT_LIST_DIR}/doc/*.rst
        ${CMAKE_CURRENT_LIST_DIR}/doc/_doxygen/*
        ${CMAKE_CURRENT_LIST_DIR}/doc/images/*.png
        ${CMAKE_CURRENT_LIST_DIR}/doc/images/*.svg
        ${CMAKE_CURRENT_LIST_DIR}/doc/_static/*
    )
    file(GLOB_RECURSE SPHINX_EXCLUDE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/doc/build/*
        ${CMAKE_CURRENT_LIST_DIR}/doc/doxygen/*
    )
    list(REMOVE_ITEM SPHINX_FILES ${SPHINX_EXCLUDE_FILES})
    list(APPEND SPHINX_FILES ${CMAKE_CURRENT_LIST_DIR}/doc/conf.py)
    set(SPHINX_INDEX_FILE ${CMAKE_CURRENT_LIST_DIR}/doc/build/index.html)

    add_custom_command(
        OUTPUT ${SPHINX_INDEX_FILE}
        DEPENDS ${DOXYGEN_INDEX_FILE} ${SPHINX_FILES}
        COMMAND doxysphinx build ${CMAKE_CURRENT_LIST_DIR}/doc ${CMAKE_CURRENT_LIST_DIR}/doc/build ${CMAKE_CURRENT_LIST_DIR}/doc/doxygen/html
        COMMAND sphinx-build -b html ${CMAKE_CURRENT_LIST_DIR}/doc ${CMAKE_CURRENT_LIST_DIR}/doc/build
        COMMAND ${CMAKE_COMMAND} -E echo "Generated sphinx documentation in: ${SPHINX_INDEX_FILE}"
        VERBATIM
    )

    add_custom_target(sphinx 
        DEPENDS ${SPHINX_INDEX_FILE}
    )

    #------------------------------------------------------------#
    #------------------- static analysis target -----------------#
    #------------------------------------------------------------#
    # Target that compiles ECU with all warnings and runs GCC static analyzer.
    # Therefore only create this target if GCC compiler is being used.
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_library(static_analysis STATIC EXCLUDE_FROM_ALL
            ${ECU_SOURCES}
        )

        target_compile_options(static_analysis
            PRIVATE
                -fanalyzer -fstack-usage -Wstack-usage=200
        )

        if(ECU_DISABLE_ASSERTS)
            target_compile_options(static_analysis
                PRIVATE
                    # Do not check for possible NULL dereference since assertions are disabled. Assertions are 
                    # the primary mechanism used to trigger errors when NULL dereferences occur.
                    -Wno-analyzer-null-dereference -Wno-analyzer-possible-null-dereference

                    # Some functions and parameters will now be unused because they are 
                    # only called within assert macros. Suppress these warnings.
                    -Wno-unused-function -Wno-unused-parameter
            )
        endif()

        target_link_libraries(static_analysis
            PRIVATE
                ecu_common
        )
    endif()

    #------------------------------------------------------------#
    #--------------------- UNIT TEST TARGETS --------------------#
    #------------------------------------------------------------#
    # Only define unit test targets if running on the native host (Linux computer).
    # Unit tests also require ECU assertions to be ENABLED. Tests frequency verify NULL dereferences, invalid
    # function parmaeters, etc. Disabling these checks invalidates the tests (and probably results in seg faults).
    if(PLATFORM STREQUAL "linux" AND NOT ECU_DISABLE_ASSERTS)
        include(CTest) # Must be in top-level directory for CTest to discover tests.....
        add_subdirectory(tests/unit)
    endif()
endif() # ECU_INTERNAL