# cpp stuff
## Build
### Requried YY repositories:
* yy_cmake
* yy_test

### Requrired 3rd party libraries
* boost 1.84 N.B. required for `yy_locale.cpp` & `yy_string_case.cpp`.
* fmt (see https://github.com/fmtlib/fmt)
* spdlog (see https://github.com/gabime/spdlog)
* Google Test (see https://github.com/google/googletest)
* Google Benchmark (see https://github.com/google/benchmark)

### CMake command
In `yy_cpp` source directiory:
```
cmake --fresh -DCMAKE_INSTALL_PREFIX=<install dir> -DYY_THIRD_PARTY_LIBRARY=<third party lib dir> -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DCMAKE_BUILD_TYPE=Release .
```

### Build unit tests & run
In `yy_cpp` source directiory:
```
make test
```
### Build benchmarks & run
In `yy_cpp` source directiory:
```
make benchmark
```
### Build and install lib & headers
```
make install
```
This will install
* `libyy_cpp.a` in `<install dir>/lib`.
* headers in `<install dir>/include/yy_cpp`
