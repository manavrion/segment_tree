cd "$(dirname "$0")"
rm -rf .build
cmake -S . -B .build -DCMAKE_BUILD_TYPE=Release
cmake --build .build --parallel 8
cd .build
mkdir install
cmake --install . --prefix install
