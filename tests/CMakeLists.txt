if(ECU_ENABLE_BUILD_TESTING)
    add_executable(ecu_build_test 
        ${CMAKE_CURRENT_LIST_DIR}/build-test/main.c
    )

    target_link_libraries(ecu_build_test
        PRIVATE 
            ecu_library        
    )
endif()
##################################################################################


if(ECU_ENABLE_UNIT_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # release-1.14.0
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(ecu_unit_test 
        ${CMAKE_CURRENT_LIST_DIR}/unit-test/test_circular_dll.cpp
    )

    target_link_libraries(ecu_unit_test
        PRIVATE 
            ecu_library
            GTest::gtest_main
            GTest::gmock_main
    )

    include(GoogleTest)
    gtest_discover_tests(ecu_unit_test)
endif()
##################################################################################