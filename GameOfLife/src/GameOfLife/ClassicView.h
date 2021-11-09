#pragma once

#include <cassert>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Base.h"

#define SHAPE_SIDE_LENGTH 4000.f
#define GREY_LEVEL 100

namespace GameOfLife
{

	const sf::Color gridColor(GREY_LEVEL, GREY_LEVEL, GREY_LEVEL, 255);

	template<size_t sideLength>
	class ClassicView : public AbstractView<sideLength>
	{
	public:
		ClassicView(EngineType& engine) :
			AbstractView<sideLength>(engine),
			m_shape(sf::RectangleShape(sf::Vector2f(SHAPE_SIDE_LENGTH, SHAPE_SIDE_LENGTH))),
			m_texture(sf::Texture())
		{
			m_shape.setFillColor(sf::Color::White);
			bool created = m_texture.create(static_cast<uint32_t>(sideLength), static_cast<uint32_t>(sideLength));
			assert(created);

			typename AbstractView<sideLength>::data_t cells;
			for (size_t i = 0; i < cells.size(); i++)
				cells[i] = !!(i & 4) * 255;

			m_texture.update(cells.data());
		}

		sf::Shape& render() override;

	private:
		sf::RectangleShape m_shape;
		sf::Texture        m_texture;
	};

	template<size_t sideLength>
	sf::Shape& ClassicView<sideLength>::render()
	{
		
		
		m_shape.setTexture(&m_texture, true);
		return m_shape;
	}
}

