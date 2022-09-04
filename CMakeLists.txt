cmake_minimum_required(VERSION 3.20.0)

project("Game of Life"
    VERSION 1.0
    LANGUAGES CXX CUDA
)

# Static linking by default
if(NOT DEFINED BUILD_SHARED_LIBS)
    message("set(BUILD_SHARED_LIBS False)")
    set(BUILD_SHARED_LIBS False)
endif()

add_subdirectory(GameOfLife)
