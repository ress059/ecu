#------------------------------------------------------------#
#---------- DEPENDENCIES SPECIFIC TO EMBEDDED TARGET --------#
#------------------------------------------------------------#
include(FetchContent)
FetchContent_Declare(
    CMSIS_5
    GIT_REPOSITORY https://github.com/ARM-software/CMSIS_5.git
    GIT_TAG        b5f0603d6a584d1724d952fd8b0737458b90d62b # v5.6.0 to be compatible with current STM32L4 CMSIS port.
)

FetchContent_Declare(
    STM32L4XX_HAL_DRIVER
    GIT_REPOSITORY https://github.com/STMicroelectronics/stm32l4xx-hal-driver.git
    GIT_TAG        fac0e64489ddc8318060e8c6b35d1655d3326ef8 # v1.13.5. Latest release.
)

FetchContent_MakeAvailable(CMSIS_5 STM32L4XX_HAL_DRIVER)

#------------------------------------------------------------#
#------------- FLAGS SPECIFIC TO EMBEDDED TARGET ------------#
#------------------------------------------------------------#
# if(NOT DEFINED CMAKE_C_BYTE_ORDER)
#     # Specify endianness in case CMake cannot detect it automatically.
#     # Define STM32L432xx for CMSIS.
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DECU_LITTLE_ENDIAN -DSTM32L432xx")
# endif()

# if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
#     # Use newlib nano for STDC library. Supply custom linker script. Output map file for debugging.
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -specs=nano.specs")
#     # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/stm32l432xc.ld\" -Wl,-Map=\"${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map\"")
# else()
#     message(FATAL_ERROR "Currently only support GNU.")
# endif()

#------------------------------------------------------------#
#--------------------- BUILD EXECUTABLE ---------------------#
#------------------------------------------------------------#
add_executable(ecu_build_test
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# target_include_directories(ecu_build_test
#     PRIVATE
#         ${cmsis_5_SOURCE_DIR}/CMSIS/Core/Include
# )

target_link_libraries(ecu_build_test
    PRIVATE
        ecu_build_test_settings
)
