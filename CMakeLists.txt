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

# PKG_CONFIG_PATH=${HOME}/code/tp/lib/pkgconfig:${HOME}/code/tp/share/pkgconfig:${PKG_CONFIG_PATH} cmake --fresh -DCMAKE_INSTALL_PREFIX=${HOME}/code/cpp/build -DYY_THIRD_PARTY_LIBRARY=${HOME}/code/tp -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DCMAKE_BUILD_TYPE=Release .
# cmake --fresh -DCMAKE_INSTALL_PREFIX=${HOME}/code/cpp/build -DYY_THIRD_PARTY_LIBRARY=${HOME}/code/tp -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DCMAKE_BUILD_TYPE=Release ..

cmake_minimum_required(VERSION 3.25)

project(yy_cpp_proj LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(YY_ROOT "${PROJECT_SOURCE_DIR}/..")
set(YY_CMAKE "${YY_ROOT}/yy_cmake")

include(${YY_CMAKE}/cmake_common.txt)
include(${YY_CMAKE}/cmake_clang_tidy.txt)

add_library(yy_cpp STATIC)

target_compile_options(yy_cpp
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_include_directories(yy_cpp
  SYSTEM PRIVATE
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_sources(yy_cpp
  PRIVATE
    yy_locale.cpp
    yy_string_case.cpp
    yy_string_util.cpp
    yy_scan_str.cpp
  PUBLIC FILE_SET HEADERS
    FILES
      yy_aho_corasick.h
      yy_arg_traits.h
      yy_assert.h
      yy_atomic_traits.hpp
      yy_atomic_wrapper.hpp
      yy_binary_search.h
      yy_bit_twiddling.h
      yy_clear_action.h
      yy_compare_util.h
      yy_constants.hpp
      yy_fast_atoi.h
      yy_find_iter_types.hpp
      yy_find_iter_util.hpp
      yy_find_raw_types.hpp
      yy_find_raw_util.hpp
      yy_find_types.hpp
      yy_find_util.hpp
      yy_flat_map.h
      yy_flat_radix_trie.h
      yy_flat_set.h
      yy_flat_trie.h
      yy_flat_trie_idx.h
      yy_fm_flat_trie_idx.h
      yy_fm_flat_trie_ptr.h
      yy_fm_radix_trie.h
      yy_fm_subject_observer.h
      yy_fm_trie.h
      yy_fm_trie_node_idx.hpp
      yy_fm_trie_node_ptr.hpp
      yy_func_traits.h
      yy_hash.h
      yy_init.h
      yy_int_util.h
      yy_iterator_idx.hpp
      yy_iterator_ptr.hpp
      yy_locale.h
      yy_lockable_value.h
      yy_lower_bound.h
      yy_make_lookup.h
      yy_observer_ptr.hpp
      yy_observer_ptr_fwd.hpp
      yy_radix_trie.h
      yy_range.hpp
      yy_ref_traits.h
      yy_ring_buffer.h
      yy_scan_str.hpp
      yy_span.h
      yy_static_flat_map.h
      yy_static_vector.h
      yy_string_case.h
      yy_string_util.h
      yy_subject_observer.h
      yy_tokenizer.h
      yy_traits_array.hpp
      yy_traits_span.hpp
      yy_traits_std.hpp
      yy_traits_static_vector.hpp
      yy_traits_string.hpp
      yy_traits_vector.hpp
      yy_trie.h
      yy_trie_common.h
      yy_trie_node_idx.hpp
      yy_trie_node_ptr.hpp
      yy_type_traits.h
      yy_types.hpp
      yy_utility.h
      yy_variant_util.h
      yy_vector.h
      yy_vector_util.h
      yy_yaml_util.h )

install(TARGETS yy_cpp
  EXPORT yy_cppTargets
  FILE_SET HEADERS DESTINATION include/yy_cpp)

install(EXPORT yy_cppTargets
  NAMESPACE yy_cpp::
  DESTINATION lib/cmake/yy_cpp)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("yy_cppConfigVersion.cmake"
  VERSION ${yy_cpp_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES "yy_cppConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/yy_cppConfigVersion.cmake"
  DESTINATION lib/cmake/yy_cpp)

add_subdirectory(unit_tests)
add_subdirectory(examples)
add_subdirectory(benchmarks)

add_yy_tidy_all(yy_cpp)
