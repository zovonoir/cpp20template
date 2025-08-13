cmake_minimum_required(VERSION 3.0)
project(cpp20template)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/googletest)


add_executable(test test.cpp)
target_link_libraries(test PRIVATE gtest gtest_main)

