cmake_minimum_required(VERSION 3.20)
project(ThreadingCmake)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC "*.cpp")
add_executable(ThreadingCmake ${SRC})
