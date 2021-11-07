#pragma once

#include <array>
#include <SFML/Graphics/Shape.hpp>

namespace GameOfLife
{
	template<size_t sideLength>
	class AbstractEngine
	{
	public:
		AbstractEngine()
		{}

		virtual void step() = 0;
		virtual bool getCellState(size_t x, size_t y) const = 0;
		virtual void setCellState(size_t x, size_t y, bool isAlive) = 0;
		virtual const std::array<int, sideLength>& getData() const = 0;
	};

	template<size_t sideLength>
	class AbstractView
	{
	public:
		AbstractView(AbstractEngine<sideLength> engine)
			: m_engine(engine) {}
		virtual sf::Shape render() = 0;
	protected:
		inline const std::array<int, sideLength>& getEngineData() const
		{
			return m_engine.getData();
		}
	private:
		AbstractEngine<sideLength> m_engine;
	};

}