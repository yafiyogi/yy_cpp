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

project(examples_yy_cpp LANGUAGES CXX)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_executable(aho_corasick
  aho_corasick.cpp )

target_include_directories(aho_corasick
  SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_compile_options(aho_corasick
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_link_libraries(aho_corasick
  fmt::fmt
  spdlog::spdlog)

add_executable(lockable_values
  lockable_values.cpp )

target_include_directories(lockable_values
  SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_compile_options(lockable_values
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_link_libraries(lockable_values
  fmt::fmt
  spdlog::spdlog)

add_executable(subject_observer
  subject_observer.cpp )

target_include_directories(subject_observer
  SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_compile_options(subject_observer
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_link_libraries(subject_observer
  fmt::fmt
  spdlog::spdlog)

add_executable(table
  table.cpp )

target_include_directories(table
  SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_compile_options(table
  PRIVATE
  "-DSPDLOG_COMPILED_LIB"
  "-DSPDLOG_FMT_EXTERNAL")

target_link_libraries(table
  fmt::fmt
  spdlog::spdlog)
