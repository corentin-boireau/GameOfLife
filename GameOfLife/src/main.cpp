#include <iostream>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "GameOfLife/Base.h"
#include "GameOfLife/Macros.h"
#include "GameOfLife/Controller.h"

#define TITLE "Game of Life"

#define ZOOM_MIN 0.01f
#define ZOOM_MAX 100.f
#define ZOOM_FACTOR_PER_SCROLL_TICK 0.25f

#define MOVE_AMOUNT_PER_SEC 500.f

class DummyView : public GameOfLife::AbstractView<10>
{
public:
    DummyView(EngineType& engine, sf::Shape& shape) 
        : GameOfLife::AbstractView<10>(engine),
          m_shape(shape) {}

    virtual sf::Shape& render() override
    {
        return m_shape;
    }

private:
    sf::Shape& m_shape;
};

class DummyEngine : public GameOfLife::AbstractEngine<10>
{
public:
    DummyEngine()
        : GameOfLife::AbstractEngine<10>(),
          m_data(data_t()) {}

    virtual void step() {};
    virtual bool getCellState(size_t x, size_t y) const { return false; };
    virtual void setCellState(size_t x, size_t y, bool isAlive) {};
    virtual const data_t& getData() const { return m_data; };

private:
    data_t m_data;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 480), TITLE);

    sf::Shape& shape = sf::RectangleShape(sf::Vector2f(100.f, 100.f));
    shape.setFillColor(sf::Color::Blue);

    DummyEngine engine = DummyEngine();
    DummyView view = DummyView(engine, shape);
    GameOfLife::Controller<10> controller = GameOfLife::Controller<10>(engine, view, window, MOVE_AMOUNT_PER_SEC, ZOOM_FACTOR_PER_SCROLL_TICK);

    return 0;
}

