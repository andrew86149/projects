cmake_minimum_required(VERSION 3.5)

project (methodicsl)

add_definitions("-std=c++17")

#set(SRC SimpleUniquePointer.cpp)
#set(SRC AutoBrake.cpp)
set(SRC asioexamples.cpp)
#set(SRC asioserver.cpp)

add_executable(${PROJECT_NAME} ${SRC})
