#------------------------------------------------------------#
#----------------------- ECU SETTINGS -----------------------#
#------------------------------------------------------------#
target_compile_features(ecu 
    PRIVATE 
        c_std_23 
)

#------------------------------------------------------------#
#---------------------- CPPUTEST SETTINGS -------------------#
#------------------------------------------------------------#
include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        6f66733f7a1affc05a5ad2126d5c5eecf82f662f # Latest master. Needed for CTest to work...
)
# Set this to ON if you want to have the CppUTests in your project as well.
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
FetchContent_MakeAvailable(CppUTest)

target_compile_options(CppUTest
    PRIVATE 
        # Save debug symbols for CppUTest.
        $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-g3>
)

#------------------------------------------------------------#
#---------------------- UNIT TEST SETTINGS ------------------#
#------------------------------------------------------------#
add_executable(ECU_UNIT_TEST
    # Main
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp 

    # Tests
    ${CMAKE_CURRENT_LIST_DIR}/src/test_asserter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_dlist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_endian.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_event.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/test_fsm.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/test_timer.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/test_tree.cpp

    # Stubs
    ${CMAKE_CURRENT_LIST_DIR}/stubs/stub_asserter.cpp
)

target_compile_features(ECU_UNIT_TEST
    PRIVATE 
        # Need C++20 concepts for our unit tests.
        cxx_std_20
)

target_include_directories(ECU_UNIT_TEST
    PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_options(ECU_UNIT_TEST
    PRIVATE
        # C++ compiler flags. We know unit tests will be C++ so we can use this generator expression.
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fdiagnostics-color=always -ffunction-sections -fdata-sections -fno-common>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wshadow>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wcast-align -Wstrict-overflow=2 -Wwrite-strings>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wcast-qual -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion>

        # Save debug symbols in our unit tests.
        $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-g3>
)

#------------------------------------------------------------#
#------------------------- LINKING --------------------------#
#------------------------------------------------------------#
target_link_libraries(ECU_UNIT_TEST
    PRIVATE 
        ecu             # Strict warnings. All of them enabled for actual library. See top CMake file.
        CppUTest        # Using namespace doesn't work. I.e. CppUTest::CppUTest
        CppUTestExt     # Using namespace doesn't work. I.e. CppUTestExt::CppUTestExt
)

#------------------------------------------------------------#
#-------------------------- CTEST ---------------------------#
#------------------------------------------------------------#
include(CppUTest)
cpputest_discover_tests(ECU_UNIT_TEST)
