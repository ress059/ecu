cmake_minimum_required(VERSION 3.20)
project(ecu VERSION 0.1.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(ECU_ENABLE_UNIT_TESTING)
    # GoogleTest requires at least C++14
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_FLAGS "-ggdb -O0")

    enable_testing()
    add_subdirectory(tests)
elseif(ECU_ENABLE_BUILD_TESTING)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)

    set(CMAKE_C_FLAGS "-Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wstrict-prototypes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-qual -Wswitch-default -Wconversion -Wunreachable-code -pedantic")

    add_subdirectory(tests)
endif()