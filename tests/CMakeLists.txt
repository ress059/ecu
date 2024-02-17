add_executable(test 
    src/main.c)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH CMAKE_PREFIX_PATH)

find_package(ECU REQUIRED)
target_link_libraries(test PRIVATE ECU::ECU)

#TODO Set compiler options -Wall, etc.