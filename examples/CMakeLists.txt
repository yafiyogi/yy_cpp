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
