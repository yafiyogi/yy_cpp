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

# PKG_CONFIG=cmake --fresh -DCMAKE_INSTALL_PREFIX=${HOME}/code/cpp/build -DYY_THIRD_PARTY_LIBRARY=${HOME}/code/tp -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DCMAKE_BUILD_TYPE=Release .

cmake_minimum_required(VERSION 3.24)

project(yy_cpp LANGUAGES CXX)

set(YY_ROOT "${PROJECT_SOURCE_DIR}/..")
set(YY_CMAKE "${YY_ROOT}/yy_cmake")

include(${YY_CMAKE}/cmake_common.txt)
include(${YY_CMAKE}/cmake_boost-1.84.0.txt)
find_package(Boost 1.84.0 REQUIRED COMPONENTS locale)

add_library(yy_cpp STATIC)

target_compile_options(yy_cpp
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_include_directories(yy_cpp
  PRIVATE
   "${YY_THIRD_PARTY_LIBRARY}/include" )

target_sources(yy_cpp
  PRIVATE
    yy_locale.cpp
    yy_string_case.cpp
    yy_string_util.cpp)

set(YY_CPP_HEADERS
  "yy_aho_corasick.h"
  "yy_arg_traits.h"
  "yy_array_traits.h"
  "yy_assert.h"
  "yy_binary_search.h"
  "yy_bit_twiddling.h"
  "yy_clear_action.h"
  "yy_flat_map.h"
  "yy_flat_radix_trie.h"
  "yy_flat_set.h"
  "yy_flat_trie.h"
  "yy_fm_flat_trie.h"
  "yy_fm_radix_trie.h"
  "yy_fm_trie.h"
  "yy_func_traits.h"
  "yy_hash.h"
  "yy_locale.h"
  "yy_lockable_value.h"
  "yy_lower_bound.h"
  "yy_make_lookup.h"
  "yy_radix_trie.h"
  "yy_ref_traits.h"
  "yy_ring_buffer.h"
  "yy_span.h"
  "yy_static_flat_map.h"
  "yy_static_vector.h"
  "yy_string_case.h"
  "yy_string_traits.h"
  "yy_string_util.h"
  "yy_subject_observer.h"
  "yy_tokenizer.h"
  "yy_trie.h"
  "yy_type_traits.h"
  "yy_utility.h"
  "yy_variant_traits.h"
  "yy_vector.h"
  "yy_vector_traits.h"
  "yy_vector_util.h" )


add_subdirectory(unit_tests)
add_subdirectory(examples)
add_subdirectory(benchmarks)

install(TARGETS yy_cpp )
install(FILES ${YY_CPP_HEADERS} DESTINATION include/yy_cpp )
