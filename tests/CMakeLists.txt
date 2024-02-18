cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH CMAKE_PREFIX_PATH) # TODO Update.
find_package(ECU REQUIRED)

if(ECU_ENABLE_UNIT_TESTING)
    add_executable(ECU_unit_test
        src/test_circular_sll.cpp
    )

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # release-1.14.0
    )

    FetchContent_MakeAvailable(googletest)

    target_link_libraries(ECU_unit_test
        PRIVATE 
            ECU::ECU
            GTest::gtest_main
            GTest::gmock_main
    )

    include(GoogleTest)
    gtest_discover_tests(ECU_unit_test)

elseif(ECU_ENABLE_BUILD_TESTING)
    add_library(ECU_build STATIC)
    target_link_libraries(ECU_build
        PRIVATE 
            ECU::ECU 
    )
endif()