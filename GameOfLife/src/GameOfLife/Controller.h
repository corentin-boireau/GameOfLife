#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "Base.h"

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
		Controller(AbstractEngine<sideLength>& engine, AbstractView<sideLength>& view, sf::RenderWindow& window,
			       float moveAmountPerSec, float zoomFactorPerScrollTick)
			: m_engine(engine), m_view(view), m_window(window)
		{
			m_windowInfos = WindowInfos(sf::Vector2f(static_cast<float>(window.getSize().x),
				                                     static_cast<float>(window.getSize().y)), 
				                        sf::Vector2f(1.f, 1.f));
			m_camera = Camera(moveAmountPerSec, zoomFactorPerScrollTick);
		}

		void mainLoop();

	private:
		AbstractEngine<sideLength>& m_engine;
		AbstractView<sideLength>&   m_view;
		sf::RenderWindow&           m_window;
		Camera                      m_camera;
		WindowInfos                 m_windowInfos;


		void         adjustTransform(sf::Shape& shape);
		void         handleKeyboardState(float timeElapsed);
		sf::Vector2f computePointCoordinates(sf::Vector2f pointOnWindow);
	};
}