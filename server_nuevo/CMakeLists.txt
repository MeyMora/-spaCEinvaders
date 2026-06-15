cmake_minimum_required(VERSION 3.20)
project(spaCEinvaders C)

set(CMAKE_C_STANDARD 99)

add_executable(game
        client_c/src/main.c
        client_c/src/network.c
)

target_include_directories(game PRIVATE
        client_c/include
        client_c/raylib/include
)

target_link_directories(game PRIVATE
        client_c/raylib/lib
)

target_link_libraries(game PRIVATE
        raylib
        opengl32
        gdi32
        winmm
        ws2_32
)