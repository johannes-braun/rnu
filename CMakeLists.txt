cmake_minimum_required(VERSION 3.13)

project(rnu LANGUAGES CXX VERSION 1.0)
add_library(rnu STATIC)
target_include_directories(rnu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(rnu PUBLIC cxx_std_23)
target_sources(rnu PUBLIC cpp.hint)
target_sources(rnu PUBLIC 
  src/thread_pool.cpp
  src/ecs.cpp
  src/system.cpp
  src/component.cpp
  src/obj.cpp
  src/font.cpp
  src/skyline_packer.cpp
  src/vector_image.cpp
  src/bvh.cpp
  src/rnu.natvis)
add_library(rnu::rnu ALIAS rnu)

option(RNU_BUILD_EXAMPLES "Build example executables." OFF)
if(RNU_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(RNU_BUILD_EXAMPLES)

option(RNU_BUILD_TESTS "Build test executables." OFF)
if(RNU_BUILD_TESTS)
  add_subdirectory(tests)
endif(RNU_BUILD_TESTS)