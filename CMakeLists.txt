cmake_minimum_required(VERSION 3.21)

# Interface library since this is meant to be compiled with the target 
# application's toolchain.
add_library(ecu_library INTERFACE)

target_sources(ecu_library
    INTERFACE 
        ${CMAKE_CURRENT_LIST_DIR}/src/asserter.c
        ${CMAKE_CURRENT_LIST_DIR}/src/circular_dll.c 
        ${CMAKE_CURRENT_LIST_DIR}/src/fsm.c
)

# Example include in the Application would be #include <ecu/fsm.h> 
# in order to give larger apps some scope when including files.
target_include_directories(ecu_library
    INTERFACE 
        ${CMAKE_CURRENT_LIST_DIR}/inc 
)

if(ECU_ENABLE_TESTING)
    add_subdirectory(tests)
endif()
