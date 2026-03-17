cmake_minimum_required(VERSION 3.5)

project (pong)

add_definitions("-std=c++20")

set(SFML_STATIC_LIBRARIES ON)

find_package(SFML 3 COMPONENTS Graphics Window Audio System Network)


include_directories(${SFML_INCLUDE_DIR})

add_executable(pong Bat.cpp Ball.cpp main.cpp)

target_link_libraries(pong ${SFML_LIBRARIES})

target_link_libraries( pong
    PRIVATE
        SFML::System
        SFML::Graphics
        SFML::Window
        SFML::Audio
)
