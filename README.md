# easy-debug-view
Simple and fast debug output viewer for Windows.

It is an super lightweight but faster alternative of [debugview](https://learn.microsoft.com/en-us/sysinternals/downloads/debugview) for user mode debug output.

Administrator role is needed when running the program.

## Build
### Configure and build using CMake
```console
cmake -S . -B build
cmake --build build --config RelWithDebInfo --target ALL_BUILD
```
### Configure, build and package using CMake
```console
cmake -S . -B build
cmake --build build --config RelWithDebInfo --target package
```
