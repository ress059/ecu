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
        CppUTest
        GIT_REPOSITORY https://github.com/cpputest/cpputest.git
        GIT_TAG        67d2dfd41e13f09ff218aa08e2d35f1c32f032a1 # release v4.0
    )
    # Set this to ON if you want to have the CppUTests in your project as well.
    set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
    FetchContent_MakeAvailable(CppUTest)

    add_executable(ecu_unit_test 
        ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/AllTests.cpp 
    )

    target_sources(ecu_unit_test 
        PRIVATE 
            # Mocks
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/mocks/mock_asserter.cpp
            # Tests
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_asserter.cpp
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_circular_dll.cpp
    )

    target_include_directories(ecu_unit_test 
        PRIVATE 
            ${CMAKE_CURRENT_LIST_DIR}/unit-test
    )

    # cpputest_normalize_test_output_location(ecu_unit_test)
    target_link_libraries(ecu_unit_test
        PRIVATE 
            ecu_library
            CppUTest        # Using namespace doesn't work. I.e. CppUTest::CppUTest
            CppUTestExt     # Using namespace doesn't work. I.e. CppUTestExt::CppUTestExt
    )
    # cpputest_buildtime_discover_tests(ecu_unit_test)
endif()
##################################################################################