cmake_minimum_required(VERSION 3.5)

project (methodicsc)

add_definitions("-std=c++20")

set(SRC count_lines.cpp serialex.cpp greater_than.cpp furmon.cpp main.cpp)

add_executable(${PROJECT_NAME} ${SRC})
#target_include_directories(${PROJECT_NAME} 
#    PRIVATE
#        ${PROJECT_SOURCE_DIR}
#)