#--------------------------------------------------------------------------#
#----------------------------- BUILD TEST SETTINGS ------------------------#
#--------------------------------------------------------------------------#
set(ECU_BUILD_NAME "ecu_build")


add_executable(${ECU_BUILD_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c 
)


# Compile our executable (main.c) with C23.
target_compile_features(${ECU_BUILD_NAME}
    PRIVATE 
        c_std_23
)



#--------------------------------------------------------------------------#
#------------------------------ MCU SPECIFIC FILES ------------------------#
#--------------------------------------------------------------------------#
add_subdirectory(${MCU})



#--------------------------------------------------------------------------#
#----------------------- ECU SETTINGS AND LINKING EXECUTABLE --------------#
#--------------------------------------------------------------------------#
target_compile_features(ecu 
    PRIVATE 
        c_std_23 
)


target_link_libraries(${ECU_BUILD_NAME}
    PRIVATE 
        ecu 
)
