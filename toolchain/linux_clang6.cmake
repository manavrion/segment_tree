list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/toolchain")
include(default_toolchain)

# Compiler
set(CMAKE_C_COMPILER clang-6.0)
set(CMAKE_CXX_COMPILER clang++-6.0)
