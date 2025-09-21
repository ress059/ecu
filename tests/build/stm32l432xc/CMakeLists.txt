#------------------------------------------------------------#
#---------- DEPENDENCIES SPECIFIC TO EMBEDDED TARGET --------#
#------------------------------------------------------------#
include(FetchContent)
FetchContent_Declare(
    CMSIS
    GIT_REPOSITORY https://github.com/ARM-software/CMSIS_6.git
    GIT_TAG        6f0a58d01aa9bd2feba212097f9afe7acd991d52 # v5.6.0 to be compatible with current STM32L4 CMSIS port.
)

FetchContent_Declare(
    CMSIS_DEVICE_L4
    GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis-device-l4.git
    GIT_TAG        a2530753e86dd326a75467d28feb92e2ba7d0df2 # v1.7.4. Latest release.
)

FetchContent_MakeAvailable(CMSIS CMSIS_DEVICE_L4)

#------------------------------------------------------------#
#------------- FLAGS SPECIFIC TO EMBEDDED TARGET ------------#
#------------------------------------------------------------#
if(NOT DEFINED CMAKE_C_BYTE_ORDER)
    # Specify endianness in case CMake cannot detect it automatically.
    # Define STM32L432xx for CMSIS.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DECU_LITTLE_ENDIAN -DSTM32L432xx")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # Use newlib nano for STDC library. Supply custom linker script. Output map file for debugging.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -specs=nano.specs")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/stm32l432xc.ld\" -Wl,-Map=\"${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map\"")
else()
    message(FATAL_ERROR "Currently only support GNU.")
endif()

#------------------------------------------------------------#
#--------------------- BUILD EXECUTABLE ---------------------#
#------------------------------------------------------------#
add_executable(ecu_build_test
    ${CMAKE_CURRENT_LIST_DIR}/bsp.c
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

target_include_directories(ecu_build_test
    PRIVATE
        ${cmsis_SOURCE_DIR}/CMSIS/Core/Include
        ${cmsis_device_l4_SOURCE_DIR}/Include
)

target_link_libraries(ecu_build_test
    PRIVATE
        ecu_build_test_settings
)
