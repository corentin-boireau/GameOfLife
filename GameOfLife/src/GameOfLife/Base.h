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
}