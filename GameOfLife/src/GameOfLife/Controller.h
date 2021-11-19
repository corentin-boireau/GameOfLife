#pragma once

#include <iostream>
#include <cassert>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Base.h"
#include "Macros.h"

#define ZOOM_MIN 0.1f
#define ZOOM_MAX 100.f

template<typename T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T> vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}


namespace GameOfLife
{
	struct WindowInfos
	{
		WindowInfos(sf::Vector2f winSize, sf::Vector2f winScale)
			: size(winSize), scale(winScale) {}

		sf::Vector2f size;
		sf::Vector2f scale;
	};

	struct Camera
	{
		Camera(float camMoveAmountPerSec, float camZoomFactorPerScrollTick)
			: position(sf::Vector2f(0.f, 0.f)), moveAmountPerSec(camMoveAmountPerSec),
			  zoom(1.f), zoomFactorPerScrollTick(camZoomFactorPerScrollTick) {}

		sf::Vector2f position;
		const float  moveAmountPerSec;
		float        zoom;
		const float  zoomFactorPerScrollTick;
	};

	template<size_t sideLength>
	class Controller
	{
	public:
		using EngineType = AbstractEngine<sideLength>;
		using ViewType = AbstractView<sideLength>;

		Controller(EngineType& engine, ViewType& view, sf::RenderWindow& window,
			       float moveAmountPerSec, float zoomFactorPerScrollTick)
			: m_engine(engine), m_view(view), m_window(window), 
			  m_windowInfos(static_cast<sf::Vector2f>(window.getSize()),
				                        sf::Vector2f(1.f, 1.f)),
			  m_camera(Camera(moveAmountPerSec, zoomFactorPerScrollTick)),
              m_autoRun(false)
        {
            bool created = m_texture.create(static_cast<uint32_t>(sideLength), static_cast<uint32_t>(sideLength));
            assert(created);
        }

		void mainLoop();

	private:
		EngineType&       m_engine;
		ViewType&         m_view;
		sf::RenderWindow& m_window;
		Camera            m_camera;
		WindowInfos       m_windowInfos;
        sf::Texture       m_texture;
        bool              m_autoRun;


        void         adjustTransform(sf::Transformable& transformable, sf::FloatRect localBounds);
		void         handleKeyboardState(float timeElapsed);
        sf::Vector2f computeWorldCoordinates(sf::Vector2f pointOnWindow);
        sf::Vector2i worldToCellCoordinates(sf::Vector2f pointInWorld);
	};

    const char* FONT_NAME = "assets/arial.ttf";

    template<size_t sideLength>
    void Controller<sideLength>::mainLoop()
    {   
        sf::Clock clock;

        sf::Font font;
        if (!font.loadFromFile(FONT_NAME))
            exit(EXIT_FAILURE);

        sf::Text fpsText;
        fpsText.setFont(font);
        fpsText.setCharacterSize(16);
        fpsText.setFillColor(sf::Color::Yellow);

        while (m_window.isOpen())
        {
            sf::Time timeElapsed = clock.restart();
            fpsText.setString(std::to_string(static_cast<int>(1.f / timeElapsed.asSeconds())));

            sf::Event event;
            while (m_window.pollEvent(event))
            {
                switch (event.type)
                {
                    case sf::Event::Closed:
                    {
                        m_window.close();
                        break;
                    }
                    case sf::Event::MouseWheelScrolled:
                    {
                        float previousZoom = m_camera.zoom;
                        float zoomFactor = pow(1.f + m_camera.zoomFactorPerScrollTick, event.mouseWheelScroll.delta);
                        m_camera.zoom *= zoomFactor;

                        if (m_camera.zoom < ZOOM_MIN)
                        {
                            m_camera.zoom = ZOOM_MIN;
                            zoomFactor = m_camera.zoom / previousZoom;
                        }
                        else if (m_camera.zoom > ZOOM_MAX)
                        {
                            m_camera.zoom = ZOOM_MAX;
                            zoomFactor = m_camera.zoom / previousZoom;
                        }

                        const sf::Vector2f curWinSize = static_cast<sf::Vector2f>(m_window.getSize());
                        const sf::Vector2f distanceFromWindowCenter = sf::Vector2f(curWinSize.x * 0.5f - event.mouseWheelScroll.x,
                                                                                   curWinSize.y * 0.5f - event.mouseWheelScroll.y);
                        const sf::Vector2f moveAmout = (distanceFromWindowCenter - m_camera.position) * (1.f - zoomFactor);

                        m_camera.position += moveAmout;

                        GOL_LOG("current zoom : " << m_camera.zoom);
                        GOL_LOG("camera position : " << m_camera.position << std::endl);
                        break;
                    }
                    case sf::Event::MouseButtonPressed:
                    {
                        if (event.mouseButton.button == sf::Mouse::Button::Left 
                         || event.mouseButton.button == sf::Mouse::Button::Right)
                        {
                            const sf::Vector2f pointOnWindow = sf::Vector2f(static_cast<float>(event.mouseButton.x),
                                                                            static_cast<float>(event.mouseButton.y));
                            GOL_LOG("point on window : " << pointOnWindow);
                            const sf::Vector2f curWinSize = static_cast<sf::Vector2f>(m_window.getSize());
                            const sf::Vector2f pointInCoordSystem = computeWorldCoordinates(pointOnWindow);
                            GOL_LOG("point in coordinate system : " << pointInCoordSystem);
                            const sf::Vector2i cellCoords = worldToCellCoordinates(pointInCoordSystem);
                            GOL_LOG("cell coordinates : " << cellCoords);
                            
                            if (cellCoords.x >= 0 && cellCoords.x < sideLength
                             && cellCoords.y >= 0 && cellCoords.y < sideLength)
                            {
                                m_engine.setCellState(cellCoords.x, cellCoords.y, event.mouseButton.button == sf::Mouse::Button::Left);
                            }
                        }
                        break;
                    }
                    case sf::Event::MouseMoved:
                    {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
                         || sf::Mouse::isButtonPressed(sf::Mouse::Right))
                        {
                            const sf::Vector2f pointOnWindow = sf::Vector2f(static_cast<float>(event.mouseMove.x),
                                                                            static_cast<float>(event.mouseMove.y));
                            GOL_LOG("point on window : " << pointOnWindow);

                            const sf::Vector2f curWinSize = static_cast<sf::Vector2f>(m_window.getSize());
                            const sf::Vector2f pointInCoordSystem = computeWorldCoordinates(pointOnWindow);
                            GOL_LOG("point in coordinate system : " << pointInCoordSystem);
                            const sf::Vector2i cellCoords = worldToCellCoordinates(pointInCoordSystem);
                            GOL_LOG("cell coordinates : " << cellCoords);
                            
                            if (cellCoords.x >= 0 && cellCoords.x < sideLength
                             && cellCoords.y >= 0 && cellCoords.y < sideLength)
                                m_engine.setCellState(cellCoords.x, cellCoords.y, sf::Mouse::isButtonPressed(sf::Mouse::Left));
                        }
                        break;
                    }
                    case sf::Event::Resized:
                    {
                        m_windowInfos.scale.x = m_windowInfos.size.x / static_cast<float>(event.size.width);
                        m_windowInfos.scale.y = m_windowInfos.size.y / static_cast<float>(event.size.height);
                        break;
                    }
                    case sf::Event::KeyReleased:
                    {
                        switch (event.key.code)
                        {
                            case sf::Keyboard::N:
                            {
                                m_engine.computeNextGeneration();
                                break;
                            }
                            case sf::Keyboard::Space:
                            {
                                m_autoRun = ! m_autoRun;
                                GOL_LOG(m_autoRun);
                                break;
                            }
                            case sf::Keyboard::C:
                            {
                                m_engine.clearCells();
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            handleKeyboardState(timeElapsed.asSeconds());

            if (m_autoRun)
                m_engine.computeNextGeneration();

            m_texture.update(m_view.computeColors().get());

            sf::Sprite sprite(m_texture);

            adjustTransform(sprite, sprite.getLocalBounds());

            m_window.clear();
            m_window.draw(sprite);
            m_window.draw(fpsText);
            m_window.display();
        }
    }

    template<size_t sideLength>
    void Controller<sideLength>::adjustTransform(sf::Transformable& transformable, sf::FloatRect localBounds)
    {
        sf::Vector2f newScale = sf::Vector2f(m_camera.zoom * m_windowInfos.scale.x,
                                             m_camera.zoom * m_windowInfos.scale.y);
        transformable.setScale(newScale);

        sf::Vector2f scaledBounds = sf::Vector2f(newScale.x * localBounds.width,
                                                 newScale.y * localBounds.height);
        // put entity on the center of the screen
        sf::Vector2f newPostion = (m_windowInfos.size - scaledBounds) * 0.5f;
        // take camera position into account 
        newPostion.x -= m_camera.position.x * m_windowInfos.scale.x;
        newPostion.y -= m_camera.position.y * m_windowInfos.scale.y;
        transformable.setPosition(newPostion);
    }

    template<size_t sideLength>
    void Controller<sideLength>::handleKeyboardState(float timeElapsedSeconds)
    {
        float moveAmount = m_camera.moveAmountPerSec * timeElapsedSeconds;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            m_camera.position.y -= moveAmount;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            m_camera.position.x -= moveAmount;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            m_camera.position.y += moveAmount;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            m_camera.position.x += moveAmount;
    }

    template<size_t sideLength>
    sf::Vector2f Controller<sideLength>::computeWorldCoordinates(sf::Vector2f pointOnWindow)
    {
        const sf::Vector2f curWinSize = sf::Vector2f(static_cast<sf::Vector2f>(m_window.getSize()));
        const sf::Vector2f distanceFromWindowCenter = curWinSize * 0.5f - pointOnWindow;
        return -(distanceFromWindowCenter - m_camera.position) / m_camera.zoom;
    }
    template<size_t sideLength>
    inline sf::Vector2i Controller<sideLength>::worldToCellCoordinates(sf::Vector2f pointInWorld)
    {
        constexpr const float halfSideLength = (float)sideLength / 2;

        return { (int)(pointInWorld.x + halfSideLength), 
                 (int)(pointInWorld.y + halfSideLength) };
    }
}