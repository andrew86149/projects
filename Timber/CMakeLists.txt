cmake_minimum_required(VERSION 3.5)

project (timber)

add_definitions("-std=c++20")

set(SFML_STATIC_LIBRARIES ON)

find_package(SFML 3 COMPONENTS Graphics Window Audio System Network)

include_directories(${SFML_INCLUDE_DIR})

add_executable(timber Timber.cpp)

target_link_libraries(timber ${SFML_LIBRARIES})

target_link_libraries( timber
    PRIVATE
        SFML::System
        SFML::Graphics
        SFML::Window
        SFML::Audio
)
