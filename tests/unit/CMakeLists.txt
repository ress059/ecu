#--------------------------------------------------------------------------#
#------------------------------- GET CPPUTEST -----------------------------#
#--------------------------------------------------------------------------#
include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        67d2dfd41e13f09ff218aa08e2d35f1c32f032a1 # release v4.0
)
# Set this to ON if you want to have the CppUTests in your project as well.
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
FetchContent_MakeAvailable(CppUTest)



#--------------------------------------------------------------------------#
#----------------------------- UNIT TEST SETTINGS -------------------------#
#--------------------------------------------------------------------------#
set(ECU_BUILD_NAME "ecu_unit_test")


add_executable(${ECU_BUILD_NAME} 
    # Main
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp 

    # Tests
    ${CMAKE_CURRENT_LIST_DIR}/src/test_asserter.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/test_circular_dll.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_endian.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_fsm.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/test_timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_tree.cpp

    # Stubs
    ${CMAKE_CURRENT_LIST_DIR}/stubs/stub_asserter.cpp
)


# Need C++20 concepts for our unit tests.
target_compile_features(${ECU_BUILD_NAME}
    PRIVATE 
        cxx_std_20
)


target_include_directories(${ECU_BUILD_NAME}
    PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)


# Be less strict about warnings for unit test source code.
# Only pass -Wall but make sure debug symbols are still saved.
target_compile_options(${ECU_BUILD_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-g3>
)



#--------------------------------------------------------------------------#
#----------------------- ECU SETTINGS AND LINKING EXECUTABLE --------------#
#--------------------------------------------------------------------------#
target_compile_features(ecu 
    PRIVATE 
        c_std_23 
)


# cpputest_normalize_test_output_location(ecu_unit_test)
target_link_libraries(${ECU_BUILD_NAME}
    PRIVATE 
        ecu             # Strict warnings. All of them enabled for actual library. See top CMake file.
        CppUTest        # Using namespace doesn't work. I.e. CppUTest::CppUTest
        CppUTestExt     # Using namespace doesn't work. I.e. CppUTestExt::CppUTestExt
)
# cpputest_buildtime_discover_tests(ecu_unit_test)