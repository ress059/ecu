#------------------------------------------------------------#
#------ BUILD EXECUTABLE FOR RUNNING ON EMBEDDED TARGET -----#
#------------------------------------------------------------#
if(NOT DEFINED CMAKE_C_BYTE_ORDER)
    # Specify endianness in case CMake cannot detect it automatically.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DECU_LITTLE_ENDIAN")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # Use newlib nano for STDC library. Supply custom linker script. Output map file for debugging.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -specs=nano.specs")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/stm32l432xc.ld\" -Wl,-Map=\"${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map\"")
else()
    message(FATAL_ERROR "Currently only support GNU.")
endif()

add_executable(ecu_build_test
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

target_link_libraries(ecu_build_test
    PRIVATE
        ecu_build_test_settings
)
