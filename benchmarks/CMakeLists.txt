#
#
#  MIT License
#
#  Copyright (c) 2024 Yafiyogi
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#
#

cmake_minimum_required(VERSION 3.24)

project(benchmark_yy_cpp LANGUAGES CXX)

add_executable(benchmark_yy_cpp
  bench_trie.cpp
  bench_radix_trie.cpp

  bench_flat_trie.cpp
  bench_flat_radix_trie.cpp

  bench_fm_trie.cpp
  bench_fm_radix_trie.cpp

  bench_fm_flat_trie_idx.cpp
  bench_fm_flat_trie_idx_word.cpp
  bench_fm_flat_trie_ptr.cpp
  bench_fm_flat_trie_ptr_word.cpp
  bench_map.cpp
  bench_uo_map.cpp

  bench_trie_lookup.cpp
  bench_bit_twiddling.cpp

  bench_yy_cpp.cpp )

target_include_directories(benchmark_yy_cpp
  PRIVATE
    ${PROJECT_SOURCE_DIR}/.. )

find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

target_compile_options(yy_cpp
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_link_directories(benchmark_yy_cpp
  PRIVATE
  "${YY_THIRD_PARTY_LIBRARY}/lib"
   /opt/local/lib)

target_link_libraries(benchmark_yy_cpp
  yy_cpp
  Boost::locale
  fmt::fmt
  spdlog::spdlog
  benchmark::benchmark
  #profiler
  )


# Test target
add_custom_target(benchmark
  COMMAND
    ./benchmark_yy_cpp)

add_dependencies(benchmark benchmark_yy_cpp)
