cmake_minimum_required(VERSION 3.14)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE manavrion)

project(segment_tree LANGUAGES CXX)

# Options
option(SEGMENT_TREE_BENCHMARKS "Build segment_tree benchmarks" OFF)
option(SEGMENT_TREE_EXAMPLES "Build segment_tree examples" OFF)
option(SEGMENT_TREE_TESTS "Build segment_tree tests" OFF)

# Export dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CCache
include(ccache)

# Google test
if(SEGMENT_TREE_TESTS OR SEGMENT_TREE_BENCHMARKS)
  include(googletest)
  enable_testing()
endif()

# Google benchmark
if(SEGMENT_TREE_BENCHMARKS)
  include(benchmark)
  enable_testing()
endif()

# Library definition
add_library(segment_tree INTERFACE)
add_library(manavrion::segment_tree ALIAS segment_tree)
target_include_directories(
  segment_tree INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
target_compile_features(segment_tree INTERFACE cxx_std_17)

if(SEGMENT_TREE_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

if(SEGMENT_TREE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(SEGMENT_TREE_TESTS)
  add_subdirectory(tests/unittests)
endif()
