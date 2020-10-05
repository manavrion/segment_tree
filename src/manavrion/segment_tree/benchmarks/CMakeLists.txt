cmake_minimum_required(VERSION 3.8)
project(benchmarks)

set(BENCHMARK_FILES build_combined.cc build.cc query_combined.cc query.cc update_combined.cc update.cc)

source_group("benchmarks" FILES ${BENCHMARK_FILES})

add_executable(benchmarks ${BENCHMARK_FILES})
set_target_properties(benchmarks PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(benchmarks segment_tree gtest benchmark_main)
add_test(benchmarks benchmarks)
