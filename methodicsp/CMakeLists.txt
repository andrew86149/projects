cmake_minimum_required(VERSION 3.5)

project (methodicsp)

add_definitions("-std=c++20")

set(SRC tincpp.cpp)

add_executable(${PROJECT_NAME} ${SRC})
