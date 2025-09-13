#------------------------------------------------------------#
#------- BUILD EXECUTABLE FOR RUNNING ON LINUX COMPUTER -----#
#------------------------------------------------------------#
add_executable(ecu_build_test
    ${CMAKE_CURRENT_LIST_DIR}/main.c 
)

target_link_libraries(ecu_build_test
    PRIVATE
        ecu_build_test_settings
)
