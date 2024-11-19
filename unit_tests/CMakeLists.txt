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

project(test_yy_cpp LANGUAGES CXX)

find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(yy_test REQUIRED)

add_executable(test_yy_cpp
  binary_search_tests.cpp
  bit_twiddling_tests.cpp
  lower_bound_tests.cpp
  ref_traits_tests.cpp
  string_util_tests.cpp
  tokenizer_tests.cpp
  variant_tests.cpp
  vector_util_tests.cpp
  vector_tests.cpp
  simple_vector_tests.cpp

  flat_map_tests.cpp
  flat_set_tests.cpp

  trie_tests.cpp
  radix_trie_tests.cpp

  flat_trie_tests.cpp
  flat_radix_trie_tests.cpp

  fm_trie_tests.cpp
  fm_radix_trie_tests.cpp

  fm_flat_trie_idx_tests.cpp
  fm_flat_trie_idx_word_tests.cpp
  fm_flat_trie_ptr_tests.cpp
  fm_flat_trie_ptr_word_tests.cpp
)

target_include_directories(test_yy_cpp
  PRIVATE
    ${PROJECT_SOURCE_DIR}/..
   "${CMAKE_INSTALL_PREFIX}/include" )

target_link_directories(test_yy_cpp
  PUBLIC
  /opt/local/lib)

target_compile_options(test_yy_cpp
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_link_libraries(test_yy_cpp
  yy_cpp
  yy_test::yy_test
  Boost::locale
  fmt::fmt
  spdlog::spdlog
  GTest::gtest
  GTest::gtest_main)

# Test target
add_custom_target(test
  COMMAND
    ./test_yy_cpp)

add_dependencies(test test_yy_cpp)

add_cmake_coverage(yy_cpp test)
add_cmake_asan(test_yy_cpp)
add_cmake_lsan(test_yy_cpp)
