#include <iostream>
#include <chrono>
#include <cmath>
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameOfLife/Base.h"
#include "GameOfLife/Macros.h"
#include "GameOfLife/Controller.h"
#include "GameOfLife/ClassicView.h"
#include "GameOfLife/CPUEngine.h"

#define TITLE "Game of Life"

#define ZOOM_MIN 0.01f
#define ZOOM_MAX 100.f
#define ZOOM_FACTOR_PER_SCROLL_TICK 0.25f

#define MOVE_AMOUNT_PER_SEC 500.f

#define SIDE_LENGTH 201

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 480), TITLE);

    GameOfLife::CPUEngine  <SIDE_LENGTH> engine;
    GameOfLife::ClassicView<SIDE_LENGTH> view(engine);
    GameOfLife::Controller <SIDE_LENGTH> controller(engine, view, window, MOVE_AMOUNT_PER_SEC, ZOOM_FACTOR_PER_SCROLL_TICK);
    
    controller.mainLoop();

    return 0;
}

