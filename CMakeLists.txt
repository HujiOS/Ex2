
cmake_minimum_required(VERSION 3.3.2)   # CMake version check
project(ex2)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 11)            # Enable c++11 standard
set(SOURCE_FILES uthreads.h uthreads.cpp threadStruct.cpp test5.cpp)
add_executable(ex2 ${SOURCE_FILES})       # Add executable target with source files listed in SOURCE_FILES variable
