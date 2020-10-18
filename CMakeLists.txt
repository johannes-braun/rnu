cmake_minimum_required(VERSION 3.13)

project(rnu LANGUAGES CXX VERSION 1.0)
add_library(rnu INTERFACE)
target_include_directories(rnu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(rnu INTERFACE cxx_std_20)
add_library(rnu::rnu ALIAS rnu)

option(RNU_BUILD_EXAMPLES "Build example executables." OFF)
if(RNU_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(RNU_BUILD_EXAMPLES)
