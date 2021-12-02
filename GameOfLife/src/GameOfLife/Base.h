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

	// INTERFACES

	template<size_t sideLength>
	class IEngine
	{
	public:
		virtual ~IEngine() {}

		virtual void setCellState(size_t x, size_t y, bool isAlive) = 0;
		virtual void computeNextGeneration() = 0;
		virtual void clearCells() = 0;
	};

	template<size_t sideLength>
	class IView
	{
	public:
		virtual ~IView() {}

		virtual cell_colors_t<sideLength> const& computeColors() const = 0;
	};


	// CPU Abstract classes (will be removed)
	template<size_t sideLength>
	class AbstractEngine : public IEngine<sideLength>
	{
	public:
		AbstractEngine() 
			: m_cellStates(cell_states_t<sideLength>()) {}
		AbstractEngine(cell_states_t<sideLength>&& initial_states) 
			: m_cellStates(std::move(initial_states)) {}

		virtual ~AbstractEngine() {}

		void setCellState(size_t x, size_t y, bool isAlive) override { m_cellStates[x + y * sideLength] = isAlive; }
		// inline bool getCellState(size_t x, size_t y) const           { return m_cellStates[x + y * sideLength]; }
		cell_states_t<sideLength> const& getCellStates() const       { return m_cellStates; }

	protected:
		cell_states_t<sideLength> m_cellStates;
	};

	template<size_t sideLength>
	class AbstractView : public IView<sideLength>
	{
	public:
		using EngineType = AbstractEngine<sideLength>;

		AbstractView(EngineType& engine)
			: m_engine(engine) {}

		virtual ~AbstractView() {}

	protected:
		inline cell_states_t<sideLength> const& getCellStates() const { return m_engine.getCellStates(); }
	private:
		EngineType& m_engine;
	};

}