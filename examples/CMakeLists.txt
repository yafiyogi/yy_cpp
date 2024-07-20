cmake_minimum_required(VERSION 3.24)

project(examples_yy_cpp LANGUAGES CXX)

add_executable(aho_corasick
  aho_corasick.cpp )

target_include_directories(aho_corasick
  PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_link_libraries(aho_corasick
  fmt::fmt)

add_executable(lockable_values
  lockable_values.cpp )

target_include_directories(lockable_values
  PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_link_libraries(lockable_values
  fmt::fmt)

add_executable(subject_observer
  subject_observer.cpp )

target_include_directories(subject_observer
  PRIVATE
    ${PROJECT_SOURCE_DIR}/..
    "${YY_THIRD_PARTY_LIBRARY}/include" )

target_link_libraries(subject_observer
  fmt::fmt)
