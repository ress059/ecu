#------------------------------------------------------------#
#---------------- INITIAL PROJECT INVOKATION ----------------#
#------------------------------------------------------------#
cmake_minimum_required(VERSION 3.21)
project(ECU_LIBRARY VERSION 0.1)
set(ECU_SUPPORTED_COMPILERS "GNU")
option(ECU_DISABLE_ASSERTS OFF)

if(NOT CMAKE_C_COMPILER_ID IN_LIST ECU_SUPPORTED_COMPILERS)
    message(WARNING "Using untested compiler. Currently supported compilers = ${ECU_SUPPORTED_COMPILERS}")
endif()

#------------------------------------------------------------#
#----------------------- COMMON SETTINGS --------------------#
#------------------------------------------------------------#
if(ECU_INTERNAL)
    add_library(common_compiler_flags INTERFACE)
    target_compile_options(common_compiler_flags
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
endif()

#------------------------------------------------------------#
#------------------------ ecu target ------------------------#
#------------------------------------------------------------#
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

if(ECU_INTERNAL)
    target_compile_options(ecu
        PRIVATE
            $<$<COMPILE_LANG_AND_ID:C,GNU>:-fstack-usage -Wstack-usage=200>
    )

    target_link_libraries(ecu 
        PRIVATE
            common_compiler_flags
    )

    # NOTE: If ECU target properties change this has to be revisited. This function copies ECU target
    # into another target in case the source code needs to be compiled differently (i.e tests with unique
    # preprocessor defines).
    function(copy_ecu_target dest)
        if(TARGET ${dest})
            message(FATAL_ERROR "dest target already exists! dest = ${dest}")
        elseif(${dest} STREQUAL "ecu")
            message(FATAL_ERROR "dest target cannot have name ecu. ecu target already exists and is what is being copied over to dest.")
        endif()

        get_target_property(ecu_sources ecu SOURCES)
        get_target_property(ecu_compile_definitions ecu COMPILE_DEFINITIONS)
        get_target_property(ecu_compile_features ecu COMPILE_FEATURES)
        get_target_property(ecu_compile_options ecu COMPILE_OPTIONS)
        get_target_property(ecu_include_directories ecu INCLUDE_DIRECTORIES)

        add_library(${dest} STATIC EXCLUDE_FROM_ALL
            ${ecu_sources}
        )

        if(NOT ecu_compile_definitions STREQUAL "ecu_compile_definitions-NOTFOUND")
            target_compile_definitions(${dest}
                PUBLIC # All ECU compile definitions are PUBLIC. Have to change if this ever changes.
                    ${ecu_compile_definitions}
            )
        endif()

        if(NOT ecu_compile_features STREQUAL "ecu_compile_features-NOTFOUND")
            target_compile_features(${dest}
                PUBLIC # All ECU compile features are PUBLIC. Have to change if this ever changes.
                    ${ecu_compile_features}
            )
        endif()

        if(NOT ecu_compile_options STREQUAL "ecu_compile_options-NOTFOUND")
            target_compile_options(${dest}
                PRIVATE # All ECU compile options are PRIVATE. Have to change if this ever changes.
                    ${ecu_compile_options}
            )
        endif()

        if(ecu_include_directories STREQUAL "ecu_include_directories-NOTFOUND")
            message(FATAL_ERROR "INCLUDE_DIRECTORIES property of ecu could not be extracted.")
        else()
            target_include_directories(${dest}
                PUBLIC # All ECU include directories are PUBLIC. Have to change if this ever changes.
                    ${ecu_include_directories}
            )
        endif()

        unset(ecu_sources)
        unset(ecu_compile_definitions)
        unset(ecu_compile_features)
        unset(ecu_compile_options)
        unset(ecu_include_directories)
    endfunction()
endif()

#------------------------------------------------------------#
#--------------------- INTERNAL TARGETS ---------------------#
#------------------------------------------------------------#
if(ECU_INTERNAL)

    #------------------------------------------------------------#
    #---------------------- doxygen target ----------------------#
    #------------------------------------------------------------#
    # Target that generates doxygen documentation locally if:
    #   - Any ECU public headers change.
    #   - Doxyfile.in changes.
    file(GLOB_RECURSE DOXYGEN_FILES 
        CONFIGURE_DEPENDS # Also run if file added or removed.
            ${CMAKE_CURRENT_LIST_DIR}/inc/*.h
    )
    set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_LIST_DIR}/doc/doxygen/html/index.html)
    list(APPEND DOXYGEN_FILES ${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile.in ${CMAKE_CURRENT_LIST_DIR}/Doxyfile @ONLY)

    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${DOXYGEN_FILES}
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
    #------------------- clang format targets -------------------#
    #------------------------------------------------------------#
    # Phony target that clang-formats source code.
    file(GLOB_RECURSE CLANG_FORMAT_FILES 
        CONFIGURE_DEPENDS # Also run if file added or removed.
            ${CMAKE_CURRENT_LIST_DIR}/inc/*.h
    )
    get_target_property(ecu_sources ecu SOURCES)
    list(APPEND CLANG_FORMAT_FILES ${ecu_sources})
    unset(ecu_sources)

    add_custom_target(clang_format
        COMMENT "Formatting all ECU files: ${CLANG_FORMAT_FILES}"
        COMMAND clang-format --Werror --fallback-style=none --ferror-limit=0 --style=file -i ${CLANG_FORMAT_FILES}
    )

    add_custom_target(clang_format_ci # Returns error code if formatting error but does not format any files.
        COMMENT "Checking for formatting errors in all ECU files: ${CLANG_FORMAT_FILES}"
        COMMAND clang-format --Werror --fallback-style=none --ferror-limit=0 --style=file --dry-run ${CLANG_FORMAT_FILES}
    )

    #------------------------------------------------------------#
    #------------------- static analysis target -----------------#
    #------------------------------------------------------------#
    # Target that runs GCC static analyzer on ECU source code.
    # Therefore only create this target if GCC compiler is being used.
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        copy_ecu_target(static_analysis)

        target_compile_options(static_analysis
            PRIVATE
                -fanalyzer
        )

        if(ECU_DISABLE_ASSERTS)
            target_compile_options(static_analysis
                PRIVATE
                    # Do not check for possible NULL dereference since assertions are disabled. Assertions are 
                    # the primary mechanism used to trigger errors when NULL dereferences occur.
                    -Wno-analyzer-null-dereference -Wno-analyzer-possible-null-dereference
            )
        endif()

        target_link_libraries(static_analysis
            PRIVATE
                common_compiler_flags
        )
    endif()

    #------------------------------------------------------------#
    #--------------------- UNIT TEST TARGETS --------------------#
    #------------------------------------------------------------#
    if(PLATFORM STREQUAL "linux") # Unit tests are only for running on native host (Linux computer).
        include(CTest) # Must be in top-level directory for CTest to discover tests.....
        add_subdirectory(tests/unit)
    endif()

    #------------------------------------------------------------#
    #----------------- INTEGRATION TEST TARGETS -----------------#
    #------------------------------------------------------------#
    # add_subdirectory(tests/integration)
endif() # ECU_INTERNAL