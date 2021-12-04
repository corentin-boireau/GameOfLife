#pragma once

#include <cassert>

#include "Base.h"
#include "Macros.h"

namespace GameOfLife
{

    // --------------------- CPU ENGINE -----------------------//
    template<size_t sideLength>
    class CPUEngine : public IEngine<sideLength>
    {
    public:
        CPUEngine()
            : m_cellStates(cell_states_t<sideLength>()),
            m_nextCellStates(cell_states_t<sideLength>()) {}
        CPUEngine(cell_states_t<sideLength>&& initial_states)
            : m_cellStates(std::move(initial_states)),
            m_nextCellStates(cell_states_t<sideLength>()) {}

        inline cell_states_t<sideLength> const& getCellStates() const { return m_cellStates; }

        void setCellState(size_t x, size_t y, bool isAlive) override { m_cellStates[x + y * sideLength] = isAlive; }
        void clearCells() override { m_cellStates.fill(false); }
        void computeNextGeneration() override;

    private:
        cell_states_t<sideLength> m_cellStates;
        cell_states_t<sideLength> m_nextCellStates;

        inline bool topLeftNeighborIndex(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;
            constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

            int64_t topLeft = (int64_t)index - lineSize - 1;
            if (index % lineSize == 0)
                topLeft += lineSize;
            if (topLeft < 0)
                topLeft += gridSize;

            return m_cellStates[topLeft];
        }

        inline bool topCenterNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;
            constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

            int64_t topCenter = (int64_t)index - lineSize;
            if (topCenter < 0)
                topCenter += gridSize;
            return m_cellStates[topCenter];
        }

        inline bool topRightNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;
            constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

            int64_t topRight = (int64_t)index - lineSize + 1;

            if (topRight % lineSize == 0)
                topRight -= lineSize;
            if (topRight < 0)
                topRight += gridSize;

            return m_cellStates[topRight];
        }

        inline bool midLeftNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;

            int64_t midLeft = (int64_t)index - 1;
            if (index % lineSize == 0)
                midLeft += lineSize;
            return m_cellStates[midLeft];
        }

        inline bool midRightNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;

            int64_t midRight = (int64_t)index + 1;
            if (midRight % lineSize == 0)
                midRight -= lineSize;
            return m_cellStates[midRight];
        }

        inline bool botLeftNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;
            constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

            int64_t botLeft = (int64_t)index + lineSize - 1;
            if (index % lineSize == 0)
                botLeft += lineSize;
            if (botLeft >= gridSize)
                botLeft -= gridSize;

            return m_cellStates[botLeft];
        }

        inline bool botCenterNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;
            constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

            int64_t botCenter = (int64_t)index + lineSize;
            if (botCenter >= gridSize)
                botCenter -= gridSize;

            return m_cellStates[botCenter];
        }

        inline bool botRightNeighborState(size_t index)
        {
            constexpr const int64_t lineSize = (int64_t)sideLength;
            constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

            int64_t botRight = (int64_t)index + lineSize + 1;
            if (botRight % lineSize == 0)
                botRight -= lineSize;
            if (botRight >= gridSize)
                botRight -= gridSize;

            return m_cellStates[botRight];
        }

    };

    template<size_t sideLength>
    void CPUEngine<sideLength>::computeNextGeneration()
    {
        // Builds the next states on m_nextCellStates
        for (size_t curIndex = 0; curIndex < m_cellStates.size(); curIndex++)
        {
            bool topLeft   = topLeftNeighborIndex(curIndex);
            bool topCenter = topCenterNeighborState(curIndex);
            bool topRight  = topRightNeighborState(curIndex);
            bool midLeft   = midLeftNeighborState(curIndex);
            bool midRight  = midRightNeighborState(curIndex);
            bool botLeft   = botLeftNeighborState(curIndex);
            bool botCenter = botCenterNeighborState(curIndex);
            bool botRight  = botRightNeighborState(curIndex);

            int nbNeighbors = topLeft + topCenter + topRight
                            + midLeft             + midRight
                            + botLeft + botCenter + botRight;

            switch (nbNeighbors)
            {
                case 2:  m_nextCellStates[curIndex] = m_cellStates[curIndex]; break;
                case 3:  m_nextCellStates[curIndex] = true;                   break;
                default: m_nextCellStates[curIndex] = false;                  break;
            }
        }


        // Makes the new generation the current generation
        std::swap(m_cellStates, m_nextCellStates);
    }
    
    

    // ---------------------- CPU VIEW -------------------------//
    template<size_t sideLength>
    class CPUView : public IView<sideLength>
    {
    public:
        using EngineType = CPUEngine<sideLength>;

        CPUView(EngineType& engine)
            : m_engine(engine),
            m_cell_colors(cell_colors_t<sideLength>()) {}

        cell_colors_t<sideLength> const& computeColors() const override;

    private:
        EngineType& m_engine;
        mutable cell_colors_t<sideLength> m_cell_colors;

        inline cell_states_t<sideLength> const& getCellStates() const { return m_engine.getCellStates(); }
    };

    template<size_t sideLength>
    cell_colors_t<sideLength> const& CPUView<sideLength>::computeColors() const
    {
        cell_states_t<sideLength> const& states = getCellStates();
        for (size_t i = 0; i < states.size(); i++)
        {
            const size_t index = 4 * i;
            const uint8_t color = states[i] * 200;
            m_cell_colors[index + 0] = 10;
            m_cell_colors[index + 1] = color;
            m_cell_colors[index + 2] = color;
            m_cell_colors[index + 3] = 255;
        }
        return m_cell_colors;
    }
}