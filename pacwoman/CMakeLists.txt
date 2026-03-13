cmake_minimum_required(VERSION 3.5)

project (pacwoman)

add_definitions("-std=c++17")

set(SFML_STATIC_LIBRARIES ON)

find_package(SFML 3 COMPONENTS Graphics Window Audio System Network)
set(SRC Animator.cpp Bonus.cpp Character.cpp Dot.cpp  Ghost.cpp PacWoman.cpp
Maze.cpp
GameState.cpp
Game.cpp 
main.cpp)

include_directories(${SFML_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
        SFML::System
        SFML::Graphics
        SFML::Window
        SFML::Audio
)
