cmake_minimum_required(VERSION 3.21)
option(ECU_DISABLE_RUNTIME_ASSERTS OFF)

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

if(ECU_DISABLE_RUNTIME_ASSERTS)
    message(WARNING "Option ECU_DISABLE_RUNTIME_ASSERTS set to ON. Runtime asserts"
                    " are being disabled. Unused function/parameter warnings will"
                    " occur when compiling since run-time assert macro calls no"
                    " longer use any of the parameters passed to it.")

    target_compile_definitions(ecu_library 
        INTERFACE 
            ECU_DISABLE_RUNTIME_ASSERTS 
    )
endif()

if(ECU_ENABLE_TESTING)
    # CTest and enable_testing has to be in the root CMakeLists.txt. 
    # Tests will not be discovered if this is in tests/CMakeLists.txt....
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
