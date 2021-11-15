#pragma once

#include "HeapArray.h"

namespace GameOfLife
{
	template<size_t sideLength>
	using cell_states_t = heap_array<bool, sideLength * sideLength>;

	template<size_t sideLength>
	using cell_colors_t = heap_array<uint8_t, sideLength* sideLength * 4>;

	template<size_t sideLength>
	class AbstractEngine
	{
	public:
		AbstractEngine() 
			: m_cell_states(cell_states_t<sideLength>()) {}
		AbstractEngine(cell_states_t<sideLength>&& initial_states) 
			: m_cell_states(std::move(initial_states)) {}

		virtual ~AbstractEngine() {}

		inline bool getCellState(size_t x, size_t y) const            { return m_cell_states[x * sideLength + y]; }
		inline void setCellState(size_t x, size_t y, bool isAlive)    { m_cell_states[x * sideLength + y] = isAlive; }
		inline cell_states_t<sideLength> const& getCellStates() const { return m_cell_states; }
		
		virtual void computeNextGeneration() = 0;

	private:
		cell_states_t<sideLength> m_cell_states;
	};

	template<size_t sideLength>
	class AbstractView
	{
	public:
		using EngineType = AbstractEngine<sideLength>;

		AbstractView(EngineType& engine)
			: m_engine(engine) {}

		virtual ~AbstractView() {}

		virtual cell_colors_t<sideLength> const& computeColors() const = 0;
	protected:
		inline cell_states_t<sideLength> const& getCellStates() const { return m_engine.getCellStates(); }
	private:
		EngineType& m_engine;
	};

}