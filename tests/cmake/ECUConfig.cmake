if(TARGET ECU::ECU)
    return()
endif()

add_library(ECU::ECU INTERFACE IMPORTED)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH ECU_ROOT)

target_include_directories(ECU::ECU 
    INTERFACE 
        "${ECU_ROOT}/inc"
)

target_sources(ECU::ECU 
    INTERFACE 
        "${ECU_ROOT}/src/asserter.c"
        "${ECU_ROOT}/src/circular_dll.c"
        "${ECU_ROOT}/src/fsm.c"
)

if(NOT DEFINED ECU_FIND_QUIETLY)
    message(STATUS "Found ECU: ${CMAKE_CURRENT_LIST_DIR}")
endif()