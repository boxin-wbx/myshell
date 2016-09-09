cmake_minimum_required(VERSION 3.2)
project(myshell)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(SOURCE_FILES main.c main.h builtin.c builtin.h utility.c utility.h)
add_executable(myshell ${SOURCE_FILES})