cmake_minimum_required(VERSION 3.5)

project (zombies)

add_definitions("-std=c++20")

set(SFML_STATIC_LIBRARIES ON)

find_package(SFML 3 COMPONENTS Graphics Window Audio System Network)

include_directories(${SFML_INCLUDE_DIR})

add_executable(zombies TextureHolder.cpp CreateBackground.cpp Bullet.cpp Player.cpp Pickup.cpp CreateHorde.cpp Zombie.cpp ZombieArena.cpp)

target_link_libraries(zombies ${SFML_LIBRARIES})

target_link_libraries( zombies
    PRIVATE
        SFML::System
        SFML::Graphics
        SFML::Window
        SFML::Audio
)
