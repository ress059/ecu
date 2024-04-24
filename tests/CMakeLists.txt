if(ECU_ENABLE_BUILD_TESTING)
    add_executable(ecu_build_test 
        ${CMAKE_CURRENT_LIST_DIR}/build-test/main.c
    )

    target_link_libraries(ecu_build_test
        PRIVATE 
            ecu        
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

    # Need C++20 concepts
    target_compile_features(ecu_unit_test
        PRIVATE 
            cxx_std_20
    )

    target_sources(ecu_unit_test 
        PRIVATE
            # Tests
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_asserter.cpp
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_circular_dll.cpp
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_endian.cpp
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_fsm.cpp
            ${CMAKE_CURRENT_LIST_DIR}/unit-test/tests/test_timer.cpp
    )

    target_include_directories(ecu_unit_test 
        PRIVATE 
            ${CMAKE_CURRENT_LIST_DIR}/unit-test
    )

    # Be less strict about warnings for unit test source code.
    # Only pass -Wall. If CMAKE_BUILD_TYPE is "debug" then -g 
    # compiler flag will automatically be set which saves debug 
    # symbols.
    target_compile_options(ecu_unit_test 
        PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-fdiagnostics-color=always>
            $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
    )

    # cpputest_normalize_test_output_location(ecu_unit_test)
    target_link_libraries(ecu_unit_test
        PRIVATE 
            ecu             # Strict warnings. All of them enabled for actual library. See top CMake file.
            CppUTest        # Using namespace doesn't work. I.e. CppUTest::CppUTest
            CppUTestExt     # Using namespace doesn't work. I.e. CppUTestExt::CppUTestExt
    )
    # cpputest_buildtime_discover_tests(ecu_unit_test)
endif()
##################################################################################