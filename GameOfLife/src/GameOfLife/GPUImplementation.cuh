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

    template<size_t sideLength>
    void GPUEngine<sideLength>::computeNextGeneration()
    {
        // Builds the next states on m_nextCellStates

        // Makes the new generation the current generation
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
        size_t i = ((size_t)blockDim.x * blockIdx.x + threadIdx.x) * 4;
        if (i < gridSize)
        {
            const uint8_t color = devCellStates[i] * 200;
            devCellColors[i + 0] = 10;
            devCellColors[i + 1] = color;
            devCellColors[i + 2] = color;
            devCellColors[i + 3] = 255;
        }
    }

    template<size_t sideLength>
    cell_colors_t<sideLength> const& GPUView<sideLength>::computeColors() const
    {
        int threadsPerBlock = 256;
        int blocksPerGrid = (gridLength<sideLength> + threadsPerBlock - 1) / threadsPerBlock;
        computeColorsOnGPU<<<blocksPerGrid, threadsPerBlock>>>(m_devCellColors, 
            getDeviceCellStates(), gridLength<sideLength> * 4);

        CUDA_ASSERT(cudaMemcpy(m_cellColors.get(), m_devCellColors, gridLength<sideLength> * 4 * sizeof(uint8_t), cudaMemcpyDeviceToHost));
        return m_cellColors;
    }
}