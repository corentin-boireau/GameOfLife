#pragma once

#include <array>
#include <SFML/Graphics/Shape.hpp>

namespace GameOfLife
{
	template<size_t sideLength>
	class AbstractEngine
	{
	public:
		using data_t = std::array<bool, sideLength* sideLength>;

		AbstractEngine()
		{}

		virtual void step() = 0;
		virtual bool getCellState(size_t x, size_t y) const = 0;
		virtual void setCellState(size_t x, size_t y, bool isAlive) = 0;
		virtual const data_t& getData() const = 0;
	};

	template<size_t sideLength>
	class AbstractView
	{
	public:
		using data_t = std::array<bool, sideLength* sideLength>;
		using EngineType = AbstractEngine<sideLength>;

		AbstractView(EngineType& engine)
			: m_engine(engine) {}
		virtual sf::Shape& render() = 0;
	protected:
		inline const data_t& getEngineData() const
		{
			return m_engine.getData();
		}
	private:
		EngineType& m_engine;
	};

}