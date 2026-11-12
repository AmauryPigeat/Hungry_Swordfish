cmake_minimum_required(VERSION 3.25)
project(hungry_swordfish LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sous Windows, SDL2 est cherché par défaut dans C:/SDL2.
# Pour un autre emplacement : cmake -DCMAKE_PREFIX_PATH=D:/libs/SDL2 ..
if (WIN32 AND NOT CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/SDL2")
endif()

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)

add_executable(hungry_swordfish
    src/main.cpp
    src/game.cpp
    src/gameSession.cpp
    src/player.cpp
    src/boidSimulation.cpp
    src/assets.cpp
    src/sdlUtils.cpp
    src/text.cpp
    src/button.cpp
    src/highScores.cpp
    src/screens/screen.cpp
    src/screens/welcomeScreen.cpp
    src/screens/nameEntryScreen.cpp
    src/screens/playingScreen.cpp
    src/screens/pausedScreen.cpp
    src/screens/gameOverScreen.cpp
    src/screens/scoresScreen.cpp
)

if (MSVC)
    target_compile_options(hungry_swordfish PRIVATE /W4)
else()
    target_compile_options(hungry_swordfish PRIVATE -Wall -Wextra -Wpedantic)
endif()

# SDL2main fournit le point d'entrée Windows ; il doit être lié avant SDL2 avec MinGW.
if (TARGET SDL2::SDL2main)
    target_link_libraries(hungry_swordfish PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(hungry_swordfish PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)
