#
#
#  MIT License
#
#  Copyright (c) 2024-2025 Yafiyogi
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

include(${YY_CMAKE}/cmake_boost-1.84.0.txt)

find_package(Boost CONFIG 1.84.0 REQUIRED COMPONENTS locale)
find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(yy_test REQUIRED)

pkg_check_modules(icu IMPORTED_TARGET icu-i18n icu-io icu-uc REQUIRED)

add_executable(test_yy_cpp
  binary_search_tests.cpp
  bit_twiddling_tests.cpp
  compare_util_tests.cpp
  fast_atoi_tests.cpp
  find_util_iter_tests.cpp
  lower_bound_tests.cpp
  ref_traits_tests.cpp
  scan_str_tests.cpp
  span_iter_tests.cpp
  span_tests.cpp
  string_case_tests.cpp
  string_util_tests.cpp
  tokenizer_tests.cpp
  variant_tests.cpp
  vector_tests.cpp
  vector_iter_tests.cpp
  simple_vector_tests.cpp
  simple_vector_iter_tests.cpp
  vector_util_tests.cpp

  flat_map_tests.cpp
  static_flat_map_tests.cpp
  flat_set_tests.cpp

  trie_tests.cpp
  trie_n1_tests.cpp
  trie_n2_tests.cpp
  trie_n3_tests.cpp
  radix_trie_tests.cpp

  flat_trie_tests.cpp
  flat_radix_trie_tests.cpp

  fm_trie_tests.cpp
  fm_radix_trie_tests.cpp

  flat_trie_idx_tests.cpp
  flat_trie_ptr_tests.cpp
  fm_flat_trie_idx_tests.cpp
  fm_flat_trie_idx_word_tests.cpp
  fm_flat_trie_ptr_tests.cpp
  fm_flat_trie_ptr_word_tests.cpp
)

add_dependencies(test_yy_cpp yy_cpp)

target_compile_options(test_yy_cpp
  PRIVATE
    "-DSPDLOG_COMPILED_LIB"
    "-DSPDLOG_FMT_EXTERNAL")

target_include_directories(test_yy_cpp
  PRIVATE
    "${PROJECT_SOURCE_DIR}/.."
    "${CMAKE_INSTALL_PREFIX}/include" )

target_include_directories(test_yy_cpp
  SYSTEM PRIVATE
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_link_directories(test_yy_cpp
  PUBLIC
    "/opt/local/lib")

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
add_cmake_asan(yy_cpp test_yy_cpp)
add_cmake_lsan(yy_cpp test_yy_cpp)
