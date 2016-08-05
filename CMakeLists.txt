cmake_minimum_required(VERSION 3.5)
project(myshell)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES main.c main.h builtin.c builtin.h utility.c utility.h)
add_executable(myshell ${SOURCE_FILES})