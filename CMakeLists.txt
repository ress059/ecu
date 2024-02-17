cmake_minimum_required(VERSION 3.20)
project(ECU VERSION 0.1.0)

if(ECU_ENABLE_TESTING)
    add_subdirectory(tests)
endif()