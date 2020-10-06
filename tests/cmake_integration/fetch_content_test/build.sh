cd "$(dirname "$0")"
cmake -S . -B .build -DCMAKE_TOOLCHAIN_FILE=toolchain/default_build_toolchain.cmake
cmake --build .build --parallel 8
