cmake_minimum_required(VERSION 3.3.2)   # CMake version check
project(ex2)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 11)            # Enable c++11 standard
set(SOURCE_FILES uthreads.h uthreads.cpp threadStruct.hpp)
add_executable(simple_example ${SOURCE_FILES})       # Add executable target with source files listed in SOURCE_FILES variable