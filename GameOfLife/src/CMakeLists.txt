add_executable(GameOfLife_CPU
    "cpu_main.cpp"
)
target_link_libraries(GameOfLife_CPU PRIVATE sfml-graphics)
add_executable(GameOfLife_GPU
    "gpu_main.cu"
)
target_link_libraries(GameOfLife_GPU PRIVATE sfml-graphics)