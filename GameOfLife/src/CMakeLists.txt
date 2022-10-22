message("Building CPU version")
add_executable(GameOfLife_CPU
    "cpu_main.cpp"
)
target_link_libraries(GameOfLife_CPU PRIVATE sfml-graphics)
if(GPU_BUILD)
    message("Building GPU version")
    enable_language(CUDA)
    add_executable(GameOfLife_GPU
        "gpu_main.cu"
    )
    target_link_libraries(GameOfLife_GPU PRIVATE sfml-graphics)
endif()