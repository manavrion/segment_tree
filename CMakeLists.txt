cmake_minimum_required(VERSION 3.14)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE manavrion)

project(segment_tree LANGUAGES CXX)

# Options
option(SEGMENT_TREE_BENCHMARKS "Build segment_tree benchmarks" OFF)
option(SEGMENT_TREE_EXAMPLES "Build segment_tree examples" OFF)
option(SEGMENT_TREE_TESTS "Build segment_tree tests" OFF)

# Export dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CCache
include(ccache)

# Google test
if(SEGMENT_TREE_TESTS OR SEGMENT_TREE_BENCHMARKS)
  include(googletest)
  enable_testing()
endif()

# Google benchmark
if(SEGMENT_TREE_BENCHMARKS)
  include(benchmark)
  enable_testing()
endif()

# Library definition
add_library(segment_tree INTERFACE)
add_library(segment_tree::segment_tree ALIAS segment_tree)
target_include_directories(
  segment_tree INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
target_compile_features(segment_tree INTERFACE cxx_std_17)

if(SEGMENT_TREE_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

if(SEGMENT_TREE_EXAMPLES)
  add_subdirectory(examples)
endif()

if(SEGMENT_TREE_TESTS)
  add_subdirectory(tests/unittests)
endif()

set(SEGMENT_TREE_VERSION 0.1)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/segment_tree-config-version.cmake
  VERSION ${SEGMENT_TREE_VERSION}
  COMPATIBILITY AnyNewerVersion)

include(GNUInstallDirs)
install(
  TARGETS segment_tree
  EXPORT segment_tree-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
  EXPORT segment_tree-targets
  FILE segment_tree-targets.cmake
  NAMESPACE manavrion::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/segment_tree)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/segment_tree-config-version.cmake
              cmake/segment_tree-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/segment_tree)
install(DIRECTORY include/manavrion DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

export(
  EXPORT segment_tree-targets
  NAMESPACE manavrion::
  FILE segment_tree-targets.cmake)
