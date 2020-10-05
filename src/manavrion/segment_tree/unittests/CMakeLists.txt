cmake_minimum_required(VERSION 3.8)
project(unittests)

set(UNITTEST_FILES test.cc)

source_group("unittests" FILES ${UNITTEST_FILES})

add_executable(unittests ${UNITTEST_FILES})
set_target_properties(unittests PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(unittests segment_tree gtest gtest_main)
add_test(unittests unittests)
