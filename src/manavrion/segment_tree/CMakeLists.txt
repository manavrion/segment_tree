cmake_minimum_required(VERSION 3.8)
project(segment_tree)

add_library(segment_tree INTERFACE)

target_compile_features(segment_tree INTERFACE cxx_std_17)
