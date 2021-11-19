#pragma once

#include <cassert>

#include "Base.h"
#include "Macros.h"

namespace GameOfLife
{
	template<size_t sideLength>
	class CPUEngine : public AbstractEngine<sideLength>
	{
	public:
		CPUEngine()
			: AbstractEngine<sideLength>(),
			  m_nextCellStates(cell_states_t<sideLength>()) {}
		CPUEngine(cell_states_t<sideLength>&& initial_states)
			: AbstractEngine<sideLength>(std::move(initial_states)),
			  m_nextCellStates(cell_states_t<sideLength>()) {}

		void computeNextGeneration() override;
		void clearCells() override;
	private:
		cell_states_t<sideLength> m_nextCellStates;
	};



	template<size_t sideLength>
	inline size_t topLeftNeighborIndex(size_t index) 
	{
		constexpr int64_t lineSize = (int64_t)sideLength;
		constexpr int64_t gridSize = (int64_t)gridLength<sideLength>;

		int64_t topLeft = (int64_t)index - lineSize - 1;
		if (index % lineSize == 0)
			topLeft += lineSize;
		if (topLeft < 0)
			topLeft += gridSize;

		return (size_t)topLeft;
	}

	template<size_t sideLength>
	inline size_t topCenterNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;
		constexpr int64_t gridSize = (int64_t)gridLength<sideLength>;

		int64_t topCenter = (int64_t)index - lineSize;
		if (topCenter < 0)
			topCenter += gridSize;
		return (size_t)topCenter;
	}

	template<size_t sideLength>
	inline size_t topRightNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;
		constexpr int64_t gridSize = (int64_t)gridLength<sideLength>;

		int64_t topRight = (int64_t)index - lineSize + 1;

		if (topRight % lineSize == 0)
			topRight -= lineSize;
		if (topRight < 0)
			topRight += gridSize;

		return (size_t)topRight;
	}

	template<size_t sideLength>
	inline size_t midLeftNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;

		int64_t midLeft = (int64_t)index - 1;
		if (index % lineSize == 0)
			midLeft += lineSize;
		return (size_t)midLeft;
	}

	template<size_t sideLength>
	inline size_t midRightNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;

		int64_t midRight = (int64_t)index + 1;
		if (midRight % lineSize == 0)
			midRight -= lineSize;
		return (size_t)midRight;
	}

	template<size_t sideLength>
	inline size_t botLeftNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;
		constexpr int64_t gridSize = (int64_t)gridLength<sideLength>;

		int64_t botLeft = (int64_t)index + lineSize - 1;
		if (index % lineSize == 0)
			botLeft += lineSize;
		if (botLeft >= gridSize)
			botLeft -= gridSize;

		return (size_t)botLeft;
	}

	template<size_t sideLength>
	inline size_t botCenterNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;
		constexpr int64_t gridSize = (int64_t)gridLength<sideLength>;

		int64_t botCenter = (int64_t)index + lineSize;
		if (botCenter >= gridSize)
			botCenter -= gridSize;

		return (size_t)botCenter;
	}

	template<size_t sideLength>
	inline size_t botRightNeighborIndex(size_t index)
	{
		constexpr int64_t lineSize = (int64_t)sideLength;
		constexpr int64_t gridSize = (int64_t)gridLength<sideLength>;

		int64_t botRight = (int64_t)index + lineSize + 1;
		if (botRight % lineSize == 0)
			botRight -= lineSize;
		if (botRight >= gridSize)
			botRight -= gridSize;

		return (size_t)botRight;
	}


	template<size_t sideLength>
	void CPUEngine<sideLength>::computeNextGeneration() 
	{
		// Builds the next states on m_nextCellStates
		for (size_t curIndex = 0; curIndex < m_cellStates.size(); curIndex++)
		{
			size_t topLeft   = topLeftNeighborIndex  <sideLength>(curIndex);
			size_t topCenter = topCenterNeighborIndex<sideLength>(curIndex);
			size_t topRight  = topRightNeighborIndex <sideLength>(curIndex);
			size_t midLeft   = midLeftNeighborIndex  <sideLength>(curIndex);
			size_t midRight  = midRightNeighborIndex <sideLength>(curIndex);
			size_t botLeft   = botLeftNeighborIndex  <sideLength>(curIndex);
			size_t botCenter = botCenterNeighborIndex<sideLength>(curIndex);
			size_t botRight  = botRightNeighborIndex <sideLength>(curIndex);

			int nbNeighbors = m_cellStates[topLeft] + m_cellStates[topCenter] + m_cellStates[topRight]
				            + m_cellStates[midLeft]                           + m_cellStates[midRight]
				            + m_cellStates[botLeft] + m_cellStates[botCenter] + m_cellStates[botRight];

			switch (nbNeighbors)
			{
				case 2  : m_nextCellStates[curIndex] = m_cellStates[curIndex]; break;
				case 3  : m_nextCellStates[curIndex] = true;                   break;
				default : m_nextCellStates[curIndex] = false;                  break;
			}
		}


		// Makes the new generation the current generation
		std::swap(m_cellStates, m_nextCellStates);
	}

	template<size_t sideLength>
	inline void CPUEngine<sideLength>::clearCells()
	{
		m_cellStates.fill(false);
	}

}