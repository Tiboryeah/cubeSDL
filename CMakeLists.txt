cmake_minimum_required(VERSION 3.29)
project(sdl2_basic C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/Users/USER/Desktop/SDL2-2.30.7/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(sdl2_basic main.c matrix.c vector.c
        display.c triangle.c mesh.c array.c)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

