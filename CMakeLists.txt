cmake_minimum_required(VERSION 3.8)

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
include(gtest)
enable_testing()

# Google benchmark
if(SEGMENT_TREE_BENCHMARKS)
  include(gbenchmark)
endif()

# The code uses the root folder to #include
include_directories("src/")

add_subdirectory(src)
