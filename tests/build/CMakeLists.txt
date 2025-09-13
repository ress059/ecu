#------------------------------------------------------------#
#----------------------- ECU SETTINGS -----------------------#
#------------------------------------------------------------#
# Do the build test in C23. Pass -Werror when compiling ECU
# source files and our build test code so CI fails when any
# build produces warnings. -Werror is purposefully not passed
# in the top-level CMake and only done here in our build tests.
target_compile_features(ecu 
    PUBLIC 
        c_std_23
)

target_compile_options(ecu_warning_flags 
    INTERFACE 
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Werror>
)

#------------------------------------------------------------#
#------------------- BUILD TEST SETTINGS --------------------#
#------------------------------------------------------------#
add_library(ecu_build_test_settings INTERFACE)

target_sources(ecu_build_test_settings
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/led_strip.c
        ${CMAKE_CURRENT_LIST_DIR}/src/led.c
)

target_include_directories(ecu_build_test_settings
    INTERFACE 
        ${CMAKE_CURRENT_LIST_DIR}/src 
)

target_link_libraries(ecu_build_test_settings 
    INTERFACE 
        ecu
        ecu_warning_flags # Have build test code use the same flags as ECU, with the addition of -Werror.
)

#------------------------------------------------------------#
#---------------- MCU SPECIFIC FILES AND MAIN.C -------------#
#------------------------------------------------------------#
add_subdirectory(${MCU})
