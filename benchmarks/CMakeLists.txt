set(BENCHMARK_FILES
    build_comb.cc
    build_quad.cc
    build.cc
    query_comb.cc
    query_quad.cc
    query.cc
    update_comb.cc
    update_quad.cc
    update.cc)

source_group("benchmarks" FILES ${BENCHMARK_FILES})

add_executable(benchmarks ${BENCHMARK_FILES})
set_target_properties(benchmarks PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(benchmarks segment_tree gtest benchmark_main)
# add_test(benchmarks benchmarks)
