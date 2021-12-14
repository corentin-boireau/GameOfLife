#include <iostream>
#include <chrono>
#include <cmath>
#include <memory>
#include <initializer_list>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

#include "GameOfLife/Base.h"
#include "GameOfLife/Macros.h"
#include "GameOfLife/Controller.h"
#include "GameOfLife/CPUImplentation.h"
#include "GameOfLife/HeapArray.h"

#include "GameOfLife/GPUImplementation.cuh"

#define ZOOM_FACTOR_PER_SCROLL_TICK 0.25f

#define MOVE_AMOUNT_PER_SEC 500.f

#define SIDE_LENGTH 500

#define GPU 1

#if GPU
    #define TITLE "Game of Life (GPU)"
#else
    #define TITLE "Game of Life (CPU)"
#endif // GPU




int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 480), TITLE);
    
    GameOfLife::cell_states_t<SIDE_LENGTH> cell_states;
    for (size_t i = 0; i < cell_states.size(); i++)
        cell_states[i] = i > cell_states.size() * 2 / 5 ? true : false;
    
#if GPU
    GameOfLife::GPUEngine   <SIDE_LENGTH> engine(std::move(cell_states));
    GameOfLife::GPUView     <SIDE_LENGTH> view(engine);
#else                       
    GameOfLife::CPUEngine   <SIDE_LENGTH> engine(std::move(cell_states));
    GameOfLife::CPUView     <SIDE_LENGTH> view(engine);
#endif

    GameOfLife::Controller  <SIDE_LENGTH> controller(engine, view, window, MOVE_AMOUNT_PER_SEC, ZOOM_FACTOR_PER_SCROLL_TICK);
    
    controller.mainLoop();
    
    return 0;
}

