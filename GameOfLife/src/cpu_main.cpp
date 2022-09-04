#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics.hpp>

#include "GameOfLife/Controller.h"
#include "GameOfLife/CPUImplentation.h"

#include "main_constants.h"

#define TITLE "Game of Life (CPU)"


int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 480), TITLE);

    GameOfLife::cell_states_t<SIDE_LENGTH> cell_states;
    for (size_t i = 0; i < cell_states.size(); i++)
        cell_states[i] = i > cell_states.size() * 2 / 5;

    GameOfLife::CPUEngine  <SIDE_LENGTH> engine(std::move(cell_states));
    GameOfLife::CPUView    <SIDE_LENGTH> view(engine);
    GameOfLife::Controller <SIDE_LENGTH> controller(engine, view, window, MOVE_AMOUNT_PER_SEC, ZOOM_FACTOR_PER_SCROLL_TICK);

    controller.mainLoop();

    return 0;
}
