#pragma once

#include "Base.h"

namespace GameOfLife
{
	template<size_t sideLength>
	class ClassicView : public AbstractView<sideLength>
	{
	public:
		ClassicView(EngineType& engine) 
			: AbstractView<sideLength>(engine),
			  m_cell_colors(cell_colors_t<sideLength>()) {}

		cell_colors_t<sideLength> const& computeColors() const override;

	private:
		mutable cell_colors_t<sideLength> m_cell_colors;
	};

	template<size_t sideLength>
	cell_colors_t<sideLength> const& ClassicView<sideLength>::computeColors() const
	{
		cell_states_t<sideLength> const& states = getCellStates();
		for (size_t i = 0; i < states.size(); i++)
		{
			const size_t index = 4 * i;
			const uint8_t color = states[i] * 200;
			m_cell_colors[index + 0] =    10;
			m_cell_colors[index + 1] = color;
			m_cell_colors[index + 2] = color;
			m_cell_colors[index + 3] =   255;
		}
		return m_cell_colors;
	}
}

