macro(add_example name)
  add_executable(${name} ${name}.cc)
  set_target_properties(${name} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(${name} segment_tree)
endmacro()

add_example(01_basic)
add_example(02_custom_functor)
add_example(03_combined_functors)
