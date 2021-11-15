#pragma once

#include "Base.h"

namespace GameOfLife
{
	template<size_t sideLength>
	class CPUEngine : public AbstractEngine<sideLength>
	{
	public:
		using AbstractEngine<sideLength>::AbstractEngine;

		void computeNextGeneration() override;
	};

	template<size_t sideLength>
	void CPUEngine<sideLength>::computeNextGeneration() {}

}