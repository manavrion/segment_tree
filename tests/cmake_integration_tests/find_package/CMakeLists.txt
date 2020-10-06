cmake_minimum_required(VERSION 3.14)

project(find_package_test LANGUAGES CXX)

# segment_tree
find_package(segment_tree REQUIRED HINTS deps/segment_tree/.build/install)

# Binary definition
add_executable(bin src/test.cc)
target_link_libraries(bin manavrion::segment_tree)
