#------------------------------------------------------------#
#------- BUILD EXECUTABLE FOR RUNNING ON LINUX COMPUTER -----#
#------------------------------------------------------------#
target_sources(ECU_BUILD_TEST
    PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/main.c 
)
