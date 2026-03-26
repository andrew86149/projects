cmake_minimum_required(VERSION 3.5)

project (methodicsg)

#add_definitions("-std=c++17")
#add_definitions("-std=c++20")
add_definitions("-std=c++23")

#set(SRC SimpleUniquePointer.cpp)
#set(SRC AutoBrake.cpp)
#set(SRC chronoex.cpp)
set(SRC generator.cpp)
#set(SRC main.cpp)
#set(SRC containers.cpp)
#set(SRC asioserver.cpp)

add_executable(${PROJECT_NAME} ${SRC})
