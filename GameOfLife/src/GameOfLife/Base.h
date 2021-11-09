#pragma once

#include <array>
#include <SFML/Graphics/Shape.hpp>

namespace GameOfLife
{
	template<size_t sideLength>
	class AbstractEngine
	{
	public:
		using data_t = std::array<sf::Uint8, sideLength* sideLength * 4>;

		AbstractEngine() {}

		virtual ~AbstractEngine() {}

		inline bool getCellState(size_t x, size_t y) const { return m_data[x * sideLength + y]; };
		inline void setCellState(size_t x, size_t y, bool isAlive) { m_data[x * sideLength + y] = isAlive; };
		inline const data_t& getData() const { return m_data; };
		
		virtual void computeNextGeneration() = 0;

	private:
		data_t m_data;
	};

	template<size_t sideLength>
	class AbstractView
	{
	public:
		using data_t = std::array<sf::Uint8, sideLength* sideLength * 4>;
		using EngineType = AbstractEngine<sideLength>;

		AbstractView(EngineType& engine)
			: m_engine(engine) {}

		virtual ~AbstractView() {}

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