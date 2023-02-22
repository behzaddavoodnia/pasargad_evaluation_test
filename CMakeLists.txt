cmake_minimum_required(VERSION 3.14)
project(daikin_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

find_package(GTest REQUIRED)
message("GTEST_INCLUDE_DIRS = ${GTEST_INCLUDE_DIRS}")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(googletest)


#add_library(myLib STATIC main.c)
add_library(myLib SHARED myprog.c ini.c)


add_executable(mainApp main.c)
target_link_libraries(mainApp myLib zmq sqlite3)

# testing
enable_testing()

add_executable(testApp testApp.cpp myprog.c ini.c)
target_link_libraries(testApp myLib gtest_main gmock_main zmq sqlite3)

