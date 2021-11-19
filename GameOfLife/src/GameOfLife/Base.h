#pragma once

#include "HeapArray.h"

namespace GameOfLife
{
	template<size_t sideLength>		
	constexpr size_t gridLength = sideLength * sideLength;

	template<size_t sideLength>
	using cell_states_t = heap_array<bool, gridLength<sideLength>>;

	template<size_t sideLength>
	using cell_colors_t = heap_array<uint8_t, gridLength<sideLength> * 4>;

	template<size_t sideLength>
	class AbstractEngine
	{
	public:
		AbstractEngine() 
			: m_cellStates(cell_states_t<sideLength>()) {}
		AbstractEngine(cell_states_t<sideLength>&& initial_states) 
			: m_cellStates(std::move(initial_states)) {}

		virtual ~AbstractEngine() {}

		inline bool getCellState(size_t x, size_t y) const            { return m_cellStates[x + y * sideLength]; }
		inline void setCellState(size_t x, size_t y, bool isAlive)    { m_cellStates[x + y * sideLength] = isAlive; }
		inline cell_states_t<sideLength> const& getCellStates() const { return m_cellStates; }
		
		virtual void computeNextGeneration() = 0;
		virtual void clearCells() = 0;

	protected:
		cell_states_t<sideLength> m_cellStates;
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