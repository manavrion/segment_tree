cmake_minimum_required(VERSION 3.14)

project(fetch_content_test LANGUAGES CXX)

# Export dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CCache
include(ccache)

# segment_tree
include(segment_tree)

# Binary definition
add_executable(bin src/test.cc)
target_link_libraries(bin segment_tree)
