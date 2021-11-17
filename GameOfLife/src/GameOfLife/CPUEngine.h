#pragma once

#include <cassert>

#include "Base.h"

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
	private:
		cell_states_t<sideLength> m_nextCellStates;
	};

	inline size_t mod(size_t dividend, size_t divisor)
	{
		int remainder = dividend;
		assert(remainder >= 0);
		assert(remainder < divisor);
		return remainder;
	}

	template<size_t sideLength>
	void CPUEngine<sideLength>::computeNextGeneration() 
	{
		// Builds the next states on m_nextCellStates
		for (size_t curIndex = 0; curIndex < m_cellStates.size(); curIndex++)
		{
			size_t topLeft   = mod(curIndex - sideLength - 1, sideLength);
			size_t topCenter = mod(curIndex - sideLength    , sideLength);
			size_t topRight  = mod(curIndex - sideLength + 1, sideLength);
			size_t midLeft   = mod(curIndex - 1             , sideLength);
			size_t midRight  = mod(curIndex + 1             , sideLength);
			size_t botLeft   = mod(curIndex + sideLength - 1, sideLength);
			size_t botCenter = mod(curIndex + sideLength    , sideLength);
			size_t botRight  = mod(curIndex + sideLength + 1, sideLength);

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

}