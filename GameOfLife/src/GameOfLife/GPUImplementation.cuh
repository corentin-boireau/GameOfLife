#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include "Base.h"

#define CUDA_ASSERT(X)\
    {cudaError_t error = X;\
    if (error != cudaSuccess)\
        std::cerr << "CUDA_ASSERT : " << cudaGetErrorString(error) << std::endl;}

namespace GameOfLife
{
    using dev_cell_states_t = bool*;
    using dev_cell_colors_t = uint8_t*;

    template<size_t sideLength>
    class GPUEngine : public IEngine<sideLength>
    {
    public:
        GPUEngine()
        {
            CUDA_ASSERT(cudaMalloc((void**)&m_devCellStates,     gridLength<sideLength> * sizeof(bool)));
            CUDA_ASSERT(cudaMalloc((void**)&m_devNextCellStates, gridLength<sideLength> * sizeof(bool)));
        }

        GPUEngine(cell_states_t<sideLength> initial_states)
        {
            CUDA_ASSERT(cudaMalloc((void**)&m_devCellStates,     gridLength<sideLength> * sizeof(bool)));
            CUDA_ASSERT(cudaMalloc((void**)&m_devNextCellStates, gridLength<sideLength> * sizeof(bool)));

            CUDA_ASSERT(cudaMemcpy(m_devCellStates, initial_states.get(), gridLength<sideLength> * sizeof(bool), cudaMemcpyHostToDevice));
        }

        inline dev_cell_states_t getDeviceCellStates() const { return m_devCellStates; }

        // setCellState() and clearCells() will do nothing for now
        void setCellState(size_t x, size_t y, bool isAlive) override {}
        void clearCells() override {}
        void computeNextGeneration() override;

    private:
        dev_cell_states_t m_devCellStates;
        dev_cell_states_t m_devNextCellStates;
    };

    template<size_t sideLength> __device__
    bool topLeftNeighborIndex(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;
        constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

        int64_t topLeft = (int64_t)index - lineSize - 1;
        if (index % lineSize == 0)
            topLeft += lineSize;
        if (topLeft < 0)
            topLeft += gridSize;

        return devCellStates[topLeft];
    }
    template<size_t sideLength> __device__
    inline bool topCenterNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;
        constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

        int64_t topCenter = (int64_t)index - lineSize;
        if (topCenter < 0)
            topCenter += gridSize;
        return devCellStates[topCenter];
    }
    template<size_t sideLength> __device__
    inline bool topRightNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;
        constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

        int64_t topRight = (int64_t)index - lineSize + 1;

        if (topRight % lineSize == 0)
            topRight -= lineSize;
        if (topRight < 0)
            topRight += gridSize;

        return devCellStates[topRight];
    }
    template<size_t sideLength> __device__
    inline bool midLeftNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;

        int64_t midLeft = (int64_t)index - 1;
        if (index % lineSize == 0)
            midLeft += lineSize;
        return devCellStates[midLeft];
    }
    template<size_t sideLength> __device__
    inline bool midRightNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;

        int64_t midRight = (int64_t)index + 1;
        if (midRight % lineSize == 0)
            midRight -= lineSize;
        return devCellStates[midRight];
    }
    template<size_t sideLength> __device__
    inline bool botLeftNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;
        constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

        int64_t botLeft = (int64_t)index + lineSize - 1;
        if (index % lineSize == 0)
            botLeft += lineSize;
        if (botLeft >= gridSize)
            botLeft -= gridSize;

        return devCellStates[botLeft];
    }
    template<size_t sideLength> __device__
    inline bool botCenterNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;
        constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

        int64_t botCenter = (int64_t)index + lineSize;
        if (botCenter >= gridSize)
            botCenter -= gridSize;

        return devCellStates[botCenter];
    }
    template<size_t sideLength> __device__
    inline bool botRightNeighborState(size_t index, dev_cell_states_t devCellStates)
    {
        constexpr const int64_t lineSize = (int64_t)sideLength;
        constexpr const int64_t gridSize = (int64_t)gridLength<sideLength>;

        int64_t botRight = (int64_t)index + lineSize + 1;
        if (botRight % lineSize == 0)
            botRight -= lineSize;
        if (botRight >= gridSize)
            botRight -= gridSize;

        return devCellStates[botRight];
    }

    template<size_t sideLength> __global__
    void computeNextGenerationOnGPU(dev_cell_states_t devCellStates, dev_cell_states_t devNextCellStates)
    {
        size_t curIndex = ((size_t)blockDim.x * blockIdx.x + threadIdx.x);
        if (curIndex < gridLength<sideLength>)
        {
            bool topLeft   = topLeftNeighborIndex   <sideLength> (curIndex, devCellStates);
            bool topCenter = topCenterNeighborState <sideLength> (curIndex, devCellStates);
            bool topRight  = topRightNeighborState  <sideLength> (curIndex, devCellStates);
            bool midLeft   = midLeftNeighborState   <sideLength> (curIndex, devCellStates);
            bool midRight  = midRightNeighborState  <sideLength> (curIndex, devCellStates);
            bool botLeft   = botLeftNeighborState   <sideLength> (curIndex, devCellStates);
            bool botCenter = botCenterNeighborState <sideLength> (curIndex, devCellStates);
            bool botRight  = botRightNeighborState  <sideLength> (curIndex, devCellStates);

            int nbNeighbors = topLeft + topCenter + topRight
                            + midLeft             + midRight
                            + botLeft + botCenter + botRight;

            switch (nbNeighbors)
            {
                case 2:  devNextCellStates[curIndex] = devCellStates[curIndex]; break;
                case 3:  devNextCellStates[curIndex] = true;                    break;
                default: devNextCellStates[curIndex] = false;                   break;
            }
        }
    }

    template<size_t sideLength>
    void GPUEngine<sideLength>::computeNextGeneration()
    {
        constexpr int threadsPerBlock = 256;
        constexpr int blocksPerGrid = (gridLength<sideLength> + threadsPerBlock - 1) / threadsPerBlock;

        // Builds the next states on m_nextCellStates
        computeNextGenerationOnGPU<sideLength> <<<blocksPerGrid, threadsPerBlock>>>
            (m_devCellStates, m_devNextCellStates);

        // Makes the new generation the current generation
        std::swap(m_devCellStates, m_devNextCellStates);
    }



    // ---------------------- GPU VIEW -------------------------//
    template<size_t sideLength>
    class GPUView : public IView<sideLength>
    {
    public:
        using EngineType = GPUEngine<sideLength>;

        GPUView(EngineType& engine)
            : m_engine(engine),
            m_cellColors(cell_colors_t<sideLength>()) 
        {
            CUDA_ASSERT(cudaMalloc((void**)&m_devCellColors, gridLength<sideLength> * 4 * sizeof(uint8_t)));

        }

        cell_colors_t<sideLength> const& computeColors() const override;

    private:
        EngineType& m_engine;
        mutable cell_colors_t<sideLength> m_cellColors;
        dev_cell_colors_t m_devCellColors;

        inline dev_cell_states_t getDeviceCellStates() const { return m_engine.getDeviceCellStates(); }
    };

    __global__
    void computeColorsOnGPU(dev_cell_colors_t devCellColors, dev_cell_states_t devCellStates, size_t gridSize)
    {
        size_t stateIndex = ((size_t)blockDim.x * blockIdx.x + threadIdx.x);
        if (stateIndex < gridSize)
        {
            size_t colorIndex = stateIndex * 4;
            const uint8_t color = devCellStates[stateIndex] * 200;
            devCellColors[colorIndex + 0] = 10;
            devCellColors[colorIndex + 1] = color;
            devCellColors[colorIndex + 2] = color;
            devCellColors[colorIndex + 3] = 255;
        }
    }

    template<size_t sideLength>
    cell_colors_t<sideLength> const& GPUView<sideLength>::computeColors() const
    {
        constexpr int threadsPerBlock = 256;
        constexpr int blocksPerGrid = (gridLength<sideLength> + threadsPerBlock - 1) / threadsPerBlock;
        computeColorsOnGPU<<<blocksPerGrid, threadsPerBlock>>>
            (m_devCellColors, getDeviceCellStates(), gridLength<sideLength> * 4);

        CUDA_ASSERT(cudaMemcpy(m_cellColors.get(), m_devCellColors, gridLength<sideLength> * 4 * sizeof(uint8_t), cudaMemcpyDeviceToHost));
        return m_cellColors;
    }
}