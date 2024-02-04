add_executable(test 
    src/main.c)

target_include_directories(test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

target_link_libraries(test
    PRIVATE
        ECU
)