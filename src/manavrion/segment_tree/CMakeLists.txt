cmake_minimum_required(VERSION 3.8)
project(segment_tree)

add_library(segment_tree INTERFACE)

target_link_libraries(segment_tree INTERFACE range-v3)
