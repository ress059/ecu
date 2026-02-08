#------------------------------------------------------------#
#----------------------- ECU SETTINGS -----------------------#
#------------------------------------------------------------#
# Do the build test in C23. Pass -Werror when compiling ECU
# source files and our build test code so CI fails when any
# build produces warnings. -Werror is purposefully not passed
# in the top-level CMake and only done here in our build tests.
target_compile_features(ecu 
    PUBLIC 
        # c_std_23
        c_std_99
)

target_compile_options(ecu 
    PUBLIC 
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Werror>
)

#------------------------------------------------------------#
#------------------- BUILD TEST SETTINGS --------------------#
#------------------------------------------------------------#
add_executable(ECU_BUILD_TEST 
    ${CMAKE_CURRENT_LIST_DIR}/src/led_strip.c
    ${CMAKE_CURRENT_LIST_DIR}/src/led.c
)

target_compile_options(ECU_BUILD_TEST
    PRIVATE
        # Save debug symbols.
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-g3>
)

target_include_directories(ECU_BUILD_TEST
    PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/src 
)

# Have build test code use the same warning flags as ECU, with the addition of -Werror.
target_link_libraries(ECU_BUILD_TEST 
    PRIVATE 
        ecu
        ecu_warning_flags
)

#------------------------------------------------------------#
#---------------- MCU SPECIFIC FILES AND MAIN.C -------------#
#------------------------------------------------------------#
add_subdirectory(${MCU})
