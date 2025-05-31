cmake_minimum_required(VERSION 3.14)

project(puzzles)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wvla -Winit-self -Wnon-virtual-dtor -Woverloaded-virtual)

add_subdirectory(2025-01-sudoku)
add_subdirectory(2025-03-mirrors)
add_subdirectory(2025-05-number-cross5)
