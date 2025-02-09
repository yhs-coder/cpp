cmake_minimum_required(VERSION 3.26)
project(memory_management)

set(CMAKE_CXX_STANDARD 20)

add_executable(memory_management base.cpp Person.cpp DynamicArray.cpp
        MemoryPool.cpp
        MemoryPool.h)
