#include <iostream>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Base.h"

#define TITLE "Game of Life"

#define ZOOM_MIN 0.01f
#define ZOOM_MAX 100.f
#define ZOOM_FACTOR_PER_SCROLL_TICK 0.25f

#define MOVE_AMOUNT_PER_SEC 500.f

#ifdef GOL_DEBUG
    #define GOL_LOG(X) std::cout << X << std::endl 
#else
    #define GOL_LOG(X)
#endif // GOL_DEBUG

void testSFML();

int main()
{
    testSFML();

    return 0;
}

struct WindowInfos
{
    sf::Vector2f size;
    sf::Vector2f scale;
};

struct Camera
{
    sf::Vector2f position;
    float        zoom;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T> vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

void adjustTransform(sf::Shape& shape, const struct Camera &camera, const struct WindowInfos &winInfos);
void handleKeyboardState(Camera& camera, float timeElapsed);
sf::Vector2f computePointCoordinates(sf::Vector2f pointOnWindow, sf::Vector2f windowSize, const Camera& camera);

void testSFML()
{
    sf::RenderWindow window(sf::VideoMode(1000, 480), TITLE);

    WindowInfos windowInfos = {
        sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)),
        sf::Vector2f(1.f, 1.f)
    };

    Camera camera = {
        sf::Vector2f(0.f, 0.f),
        1.f
    };

    //sf::Shape& shape = sf::CircleShape(100.f); 
    sf::Shape& shape = sf::RectangleShape(sf::Vector2f(100.f, 100.f));
    shape.setFillColor(sf::Color::Blue);

    sf::Clock clock;
    
    while (window.isOpen())
    {
        sf::Time timeElapsed = clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed :
                {
                    window.close();
                    break;
                }
                case sf::Event::MouseWheelScrolled :
                {
                    float previousZoom = camera.zoom;
                    float zoomFactor = pow(1.f + ZOOM_FACTOR_PER_SCROLL_TICK, event.mouseWheelScroll.delta);
                    camera.zoom *= zoomFactor;

                    if (camera.zoom < ZOOM_MIN)
                    {
                        camera.zoom = ZOOM_MIN;
                        zoomFactor = camera.zoom / previousZoom;
                    }
                    else if (camera.zoom > ZOOM_MAX)
                    {
                        camera.zoom = ZOOM_MAX;
                        zoomFactor = camera.zoom / previousZoom;
                    }

                    const sf::Vector2f curWinSize = sf::Vector2f(static_cast<float>(window.getSize().x),
                                                                 static_cast<float>(window.getSize().y));
                    const sf::Vector2f distanceFromCenter = sf::Vector2f(curWinSize.x * 0.5f - event.mouseWheelScroll.x,
                                                                         curWinSize.y * 0.5f - event.mouseWheelScroll.y);
                    const sf::Vector2f moveAmout = (distanceFromCenter - camera.position) * (1.f - zoomFactor);
                    
                    camera.position += moveAmout;

                    GOL_LOG("current zoom : "       << camera.zoom);
                    GOL_LOG("camera position : "    << camera.position << std::endl);
                    break;
                }
                case sf::Event::MouseButtonPressed :
                {
                    const sf::Vector2f pointOnWindow = sf::Vector2f(static_cast<float>(event.mouseButton.x),
                                                                    static_cast<float>(event.mouseButton.y));
                    GOL_LOG("point on window : " << pointOnWindow);

                    const sf::Vector2f curWinSize = sf::Vector2f(static_cast<float>(window.getSize().x),
                                                                 static_cast<float>(window.getSize().y));
                    const sf::Vector2f pointInCoordSystem = computePointCoordinates(pointOnWindow, curWinSize, camera);
                    GOL_LOG("point in coordinate system : " << pointInCoordSystem);
                    break;
                }
                case sf::Event::Resized :
                {
                    windowInfos.scale.x = windowInfos.size.x / static_cast<float>(event.size.width);
                    windowInfos.scale.y = windowInfos.size.y / static_cast<float>(event.size.height);
                    break;
                }
            }
        }

        handleKeyboardState(camera, timeElapsed.asSeconds());

        // Shapes update
        adjustTransform(shape, camera, windowInfos);

        window.clear();
        window.draw(shape);
        window.display();
    }
}

void adjustTransform(sf::Shape &shape, const Camera &camera, const WindowInfos &winInfos)
{
    sf::Vector2f newScale = sf::Vector2f(camera.zoom * winInfos.scale.x,
                                         camera.zoom * winInfos.scale.y);
    shape.setScale(newScale);

    sf::Vector2f scaledBounds = sf::Vector2f(newScale.x * shape.getLocalBounds().width,
                                             newScale.y * shape.getLocalBounds().height);
    sf::Vector2f newPostion = (winInfos.size - scaledBounds) * 0.5f; // put shape on the center of the screen
    // take camera position into account 
    newPostion.x -= camera.position.x * winInfos.scale.x;
    newPostion.y -= camera.position.y * winInfos.scale.y;
    shape.setPosition(newPostion);
}

void handleKeyboardState(Camera& camera, float timeElapsedSeconds)
{
    float moveAmount = MOVE_AMOUNT_PER_SEC * timeElapsedSeconds;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        camera.position.y -= moveAmount; 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        camera.position.x -= moveAmount;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        camera.position.y += moveAmount;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        camera.position.x += moveAmount;
}

sf::Vector2f computePointCoordinates(sf::Vector2f pointOnWindow, sf::Vector2f windowSize, const Camera &camera)
{
    const sf::Vector2f distanceFromWindowCenter = windowSize * 0.5f - pointOnWindow;
    return - (distanceFromWindowCenter - camera.position) / camera.zoom;
}