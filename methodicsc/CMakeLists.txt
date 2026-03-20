cmake_minimum_required(VERSION 3.5)

project (methodicsc)

add_definitions("-std=c++20")

set(SRC count_lines.cpp)

add_executable(${PROJECT_NAME} ${SRC})
