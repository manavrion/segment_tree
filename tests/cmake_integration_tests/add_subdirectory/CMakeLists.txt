cmake_minimum_required(VERSION 3.14)

project(add_subdirectory_test LANGUAGES CXX)

# segment_tree
add_subdirectory(deps/segment_tree)

# Binary definition
add_executable(bin src/test.cc)
target_link_libraries(bin manavrion::segment_tree)
