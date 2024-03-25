# easy-debug-view
Simple and fast debug output viewer for Windows.

Administrator role is needed when running the program.

## Build
### Configure and build using CMake
```console
cmake -S . -B build
cmake --build build --config RelWithDebInfo --target ALL_BUILD
```
### Configure, build and package using CMake
cmake -S . -B build
cmake --build build --config RelWithDebInfo --target package
