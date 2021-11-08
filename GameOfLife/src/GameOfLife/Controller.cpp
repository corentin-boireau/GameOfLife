//#include "Controller.h"
//
//#include <iostream>
//#include <SFML/Window/Event.hpp>
//
//#include "Base.h"
//#include "Macros.h"
//
//#define ZOOM_MIN 0.01f
//#define ZOOM_MAX 100.f
//
//template<typename T>
//std::ostream& operator<<(std::ostream& os, sf::Vector2<T> vec)
//{
//    os << "(" << vec.x << ", " << vec.y << ")";
//    return os;
//}
//
//
//namespace GameOfLife
//{
//    template<size_t sideLength>
//    void Controller<sideLength>::mainLoop()
//    {
//        sf::Shape& shape = m_view.render();
//
//        sf::Clock clock;
//
//        while (m_window.isOpen())
//        {
//            sf::Time timeElapsed = clock.restart();
//
//            sf::Event event;
//            while (m_window.pollEvent(event))
//            {
//                switch (event.type)
//                {
//                case sf::Event::Closed:
//                {
//                    m_window.close();
//                    break;
//                }
//                case sf::Event::MouseWheelScrolled:
//                {
//                    float previousZoom = m_camera.zoom;
//                    float zoomFactor = pow(1.f + m_camera.zoomFactorPerScrollTick, event.mouseWheelScroll.delta);
//                    m_camera.zoom *= zoomFactor;
//
//                    if (m_camera.zoom < ZOOM_MIN)
//                    {
//                        m_camera.zoom = ZOOM_MIN;
//                        zoomFactor = m_camera.zoom / previousZoom;
//                    }
//                    else if (m_camera.zoom > ZOOM_MAX)
//                    {
//                        m_camera.zoom = ZOOM_MAX;
//                        zoomFactor = m_camera.zoom / previousZoom;
//                    }
//
//                    const sf::Vector2f curWinSize = sf::Vector2f(static_cast<float>(m_window.getSize().x),
//                                                                 static_cast<float>(m_window.getSize().y));
//                    const sf::Vector2f distanceFromWindowCenter = sf::Vector2f(curWinSize.x * 0.5f - event.mouseWheelScroll.x,
//                                                                               curWinSize.y * 0.5f - event.mouseWheelScroll.y);
//                    const sf::Vector2f moveAmout = (distanceFromWindowCenter - m_camera.position) * (1.f - zoomFactor);
//
//                    m_camera.position += moveAmout;
//
//                    GOL_LOG("current zoom : " << m_camera.zoom);
//                    GOL_LOG("camera position : " << m_camera.position << std::endl);
//                    break;
//                }
//                case sf::Event::MouseButtonPressed:
//                {
//                    const sf::Vector2f pointOnWindow = sf::Vector2f(static_cast<float>(event.mouseButton.x),
//                                                                    static_cast<float>(event.mouseButton.y));
//                    GOL_LOG("point on window : " << pointOnWindow);
//
//                    const sf::Vector2f curWinSize = sf::Vector2f(static_cast<float>(m_window.getSize().x),
//                                                                 static_cast<float>(m_window.getSize().y));
//                    const sf::Vector2f pointInCoordSystem = computePointCoordinates(pointOnWindow);
//                    GOL_LOG("point in coordinate system : " << pointInCoordSystem);
//                    break;
//                }
//                case sf::Event::Resized:
//                {
//                    m_windowInfos.scale.x = m_windowInfos.size.x / static_cast<float>(event.size.width);
//                    m_windowInfos.scale.y = m_windowInfos.size.y / static_cast<float>(event.size.height);
//                    break;
//                }
//                }
//            }
//
//            handleKeyboardState(timeElapsed.asSeconds());
//
//            // Shapes update
//            adjustTransform(shape);
//
//            m_window.clear();
//            m_window.draw(shape);
//            m_window.display();
//        }
//    }
//
//    template<size_t sideLength>
//    void Controller<sideLength>::adjustTransform(sf::Shape& shape)
//    {
//        sf::Vector2f newScale = sf::Vector2f(m_camera.zoom * m_windowInfos.scale.x,
//                                             m_camera.zoom * m_windowInfos.scale.y);
//        shape.setScale(newScale);
//
//        sf::Vector2f scaledBounds = sf::Vector2f(newScale.x * shape.getLocalBounds().width,
//                                                 newScale.y * shape.getLocalBounds().height);
//        // put shape on the center of the screen
//        sf::Vector2f newPostion = (m_windowInfos.size - scaledBounds) * 0.5f; 
//        // take camera position into account 
//        newPostion.x -= m_camera.position.x * m_windowInfos.scale.x;
//        newPostion.y -= m_camera.position.y * m_windowInfos.scale.y;
//        shape.setPosition(newPostion);
//    }
//
//    template<size_t sideLength>
//    void Controller<sideLength>::handleKeyboardState(float timeElapsedSeconds)
//    {
//        float moveAmount = m_camera.moveAmountPerSec * timeElapsedSeconds;
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
//            m_camera.position.y -= moveAmount;
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
//            m_camera.position.x -= moveAmount;
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
//            m_camera.position.y += moveAmount;
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
//            m_camera.position.x += moveAmount;
//    }
//
//    template<size_t sideLength>
//    sf::Vector2f Controller<sideLength>::computePointCoordinates(sf::Vector2f pointOnWindow)
//    {
//        const sf::Vector2f curWinSize = sf::Vector2f(static_cast<float>(m_window.getSize().x),
//                                                     static_cast<float>(m_window.getSize().y));
//        const sf::Vector2f distanceFromWindowCenter = curWinSize * 0.5f - pointOnWindow;
//        return -(distanceFromWindowCenter - m_camera.position) / m_camera.zoom;
//    }
//}