cmake_minimum_required(VERSION 3.8)
project(examples)

set(EXAMPLE_FILES usage.cc)

source_group("examples" FILES ${EXAMPLE_FILES})

add_executable(examples ${EXAMPLE_FILES})
set_target_properties(examples PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(examples segment_tree gtest gtest_main)
add_test(examples examples)
