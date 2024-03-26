# easy-debug-view

Simple and fast debug output viewer for Windows.

It is a super lightweight but faster alternative of [debugview](https://learn.microsoft.com/en-us/sysinternals/downloads/debugview) for user mode debug output. 

The program is console based and all the output can be redirected to a file.

```console
easy-debug-view.exe>log.txt
```

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
