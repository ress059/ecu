# TODO: config-file package.

cmake_minimum_required(VERSION 3.20)
project(ECU VERSION 0.1.0)

# TODO: Set these only if they aren't defined yet. User meant to compile library on THEIR target.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)


add_library(${PROJECT_NAME} INTERFACE)


target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/>
        $<INSTALL_INTERFACE:include>
)


target_sources(${PROJECT_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asserter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/circular_sll.c   
)




# # CMakeLists.txt for YourLibrary

# # Specify the minimum CMake version required
# cmake_minimum_required(VERSION 3.12)

# # Set the project name
# project(YourLibrary VERSION 1.0.0)

# # Set C++ standard (change as needed)
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Define the library target
# add_library(YourLibrary INTERFACE)

# # Set include directories for users
# target_include_directories(YourLibrary
#     INTERFACE
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#         $<INSTALL_INTERFACE:include>
# )

# # Optionally, you can add source files here
# # target_sources(YourLibrary
# #     INTERFACE
# #         ${CMAKE_CURRENT_SOURCE_DIR}/src/your_source_file.cpp
# # )

# # Optionally, you can add compile options or other settings here
# # target_compile_options(YourLibrary
# #     INTERFACE
# #         -Wall
# # )

# # Optionally, you can add dependencies here using target_link_libraries

# # Install the library
# install(TARGETS YourLibrary
#     EXPORT YourLibraryTargets
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION include
# )

# # Install header files
# install(DIRECTORY include/ DESTINATION include)

# # Generate and install CMake package configuration files
# include(CMakePackageConfigHelpers)

# write_basic_package_version_file(
#     "${CMAKE_CURRENT_BINARY_DIR}/YourLibraryConfigVersion.cmake"
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY AnyNewerVersion
# )

# configure_package_config_file(
#     "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
#     "${CMAKE_CURRENT_BINARY_DIR}/YourLibraryConfig.cmake"
#     INSTALL_DESTINATION lib/cmake/YourLibrary
# )

# install(
#     EXPORT YourLibraryTargets
#     FILE YourLibraryTargets.cmake
#     DESTINATION lib/cmake/YourLibrary
# )

# install(
#     FILES
#         "${CMAKE_CURRENT_BINARY_DIR}/YourLibraryConfig.cmake"
#         "${CMAKE_CURRENT_BINARY_DIR}/YourLibraryConfigVersion.cmake"
#     DESTINATION lib/cmake/YourLibrary
# )