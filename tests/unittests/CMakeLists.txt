set(UNITTEST_FILES complicated_functor_test.cc integration_test.cc lite_test.cc
                   simple_functor_test.cc)

source_group("unittests" FILES ${UNITTEST_FILES})

add_executable(unittests ${UNITTEST_FILES})
set_target_properties(unittests PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(unittests segment_tree gtest gtest_main)
add_test(unittests unittests)
