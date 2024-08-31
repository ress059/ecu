cmake_minimum_required(VERSION 3.21)
project(ECU_LIBRARY VERSION 0.1)
set(ECU_SUPPORTED_COMPILERS "GNU")
option(ECU_DISABLE_RUNTIME_ASSERTS OFF)


if(NOT CMAKE_C_COMPILER_ID IN_LIST ECU_SUPPORTED_COMPILERS)
    message(WARNING "Using untested compiler. Currently supported compilers = ${ECU_SUPPORTED_COMPILERS}")
endif()


# Note this library is meant to be compiled with the target 
# application's toolchain.
add_library(ecu STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/asserter.c
    ${CMAKE_CURRENT_LIST_DIR}/src/circular_dll.c
    ${CMAKE_CURRENT_LIST_DIR}/src/event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/fsm.c
    # ${CMAKE_CURRENT_LIST_DIR}/src/hsm.c
    ${CMAKE_CURRENT_LIST_DIR}/src/object_id.c
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tree.c
)


# Example include in the Application would be #include <ecu/fsm.h> 
# in order to give larger apps some scope when including files.
target_include_directories(ecu
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/inc 
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


# Enable all warnings and relevant debug info. Only turn these flags on when 
# ecu library is being compiled by an application using ecu. Do not propogate 
# these flags to the remaining application source code. Generator expressions
# ensures flags are always compatible with the target application's compiler.
# Currently only support GCC but it is OK if application is not using GCC. 
# In this case no flags will be used since we cannot assume anything about 
# which compiler the application will be using. ALSO NOTE: An optimization 
# flag is not specified on purpose so it can be defined by the application. 
# I.e. after importing ecu the application can do: target_compile_options(ecu PRIVATE -O2).
# It is OK if the application does not specify an optimization level - GCC
# will default it to -O0.
target_compile_options(ecu 
    PRIVATE
        # C compiler flags. We know ECU is a C library so we can use this generator expression.
        $<$<C_COMPILER_ID:GNU>:-fdiagnostics-color=always -ffunction-sections -fdata-sections -fstack-usage -fno-common>
        $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wshadow -Wstack-usage=200>
        $<$<C_COMPILER_ID:GNU>:-Wcast-align -Wstrict-prototypes -Wstrict-overflow=2 -Wwrite-strings -Waggregate-return>
        $<$<C_COMPILER_ID:GNU>:-Wcast-qual -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion>

        # Compiler flags specific to Debug builds
        $<$<AND:$<CONFIG:Debug>,$<C_COMPILER_ID:GNU>>:-g3>
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

    # Some functions will now be unused because they are called within 
    # assert macros. Use -Wno to suppress these warnings.
    target_compile_options(ecu
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-Wno-unused-function>
    )
endif()


# CTest and enable_testing has to be in the root CMakeLists.txt. 
# Tests will not be discovered if this is in tests/CMakeLists.txt....
if(ECU_ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
