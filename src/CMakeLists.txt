cmake_minimum_required(VERSION 3.8)
project(src)

add_subdirectory(manavrion/segment_tree)
add_subdirectory(manavrion/segment_tree/examples)

if(SEGMENT_TREE_BENCHMARKS)
  add_subdirectory(manavrion/segment_tree/benchmarks)
endif()

add_subdirectory(manavrion/segment_tree/unittests)
