#------------------------------------------------------------#
#-------------------------- CPPUTEST ------------------------#
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
        $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-O0 -g3>
)

target_compile_options(CppUTestExt
    PRIVATE 
        # Save debug symbols for CppUTestExt.
        $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-O0 -g3>
)

#------------------------------------------------------------#
#-------------------- ECU UNIT TEST TARGET ------------------#
#------------------------------------------------------------#
# Target that compiles ECU for unit tests. Applies unit test-specific compiler options and settings.
copy_ecu_target(ecu_unit_test_lib)

target_compile_definitions(ecu_unit_test_lib
    PUBLIC
        ECU_UNIT_TEST # PUBLIC since this may be used in ECU header files.
)

target_compile_options(ecu_unit_test_lib
    PRIVATE
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-O0 --coverage -fprofile-abs-path> # Additional flags for unit tests.
)

#------------------------------------------------------------#
#--------------- UNIT TEST EXECUTABLE TARGET ----------------#
#------------------------------------------------------------#
# Target that builds unit test executable.
add_executable(unit_test_exe EXCLUDE_FROM_ALL
    # Main
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp 

    # Tests
    ${CMAKE_CURRENT_LIST_DIR}/src/test_asserter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_dlist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_endian.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_event.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_fsm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_hsm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_ntnode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test_utils.cpp

    # Stubs
    ${CMAKE_CURRENT_LIST_DIR}/stubs/stub_asserter.cpp
)

target_compile_features(unit_test_exe
    PRIVATE 
        cxx_std_20 # Need C++20 concepts for our unit tests.
)

target_compile_options(unit_test_exe
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-O0>
        # See https://cpputest.github.io/manual.html "Resolving conflicts with STL" section.
        $<$<CXX_COMPILER_ID:GNU>:-include${CMAKE_CURRENT_LIST_DIR}/inc/cpputest_stdlib.hpp>
)

target_include_directories(unit_test_exe
    PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(unit_test_exe
    PRIVATE 
        CppUTest
        CppUTestExt
        common_compiler_flags
        ecu_unit_test_lib
)

target_link_options(unit_test_exe
    PRIVATE
        $<$<OR:$<LINK_LANG_AND_ID:C,GNU>,$<LINK_LANG_AND_ID:CXX,GNU>>:--coverage>
)

#------------------------------------------------------------#
#---------------------------- CTEST -------------------------#
#------------------------------------------------------------#
include(CTest)
include(CppUTest)
cpputest_discover_tests(unit_test_exe)

#------------------------------------------------------------#
#----------------------- unit_test target -------------------#
#------------------------------------------------------------#
# Target that builds and runs the unit test executable for easier invokation.
add_custom_target(unit_test
    COMMAND unit_test_exe
    DEPENDS unit_test_exe
    USES_TERMINAL
)

#------------------------------------------------------------#
#----------------------- gcovr target -----------------------#
#------------------------------------------------------------#
# Target that generates local html code coverage report by building
# unit test executable, running it, then running gcovr to generate report.
# Only regenerate coverage if:
#   - unit_test_exe target becomes out of date. I.e. source file(s) under test changes.
set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
set(COVERAGE_FILE ${COVERAGE_DIR}/coverage.html)

add_custom_command(
    OUTPUT ${COVERAGE_FILE}
    DEPENDS unit_test_exe
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Running unit test executable to generate .gcda files..."
    COMMAND unit_test_exe
    COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR} # Make directory in case it does not exist.
    COMMAND ${CMAKE_COMMAND} -E echo "Running gcovr to generate coverage..."
    COMMAND gcovr --root ${PROJECT_SOURCE_DIR}/src --html-details ${COVERAGE_FILE} ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "Generated coverage report in: ${COVERAGE_DIR}/coverage.html"
    VERBATIM
)

add_custom_target(gcovr
    DEPENDS ${COVERAGE_FILE}
)