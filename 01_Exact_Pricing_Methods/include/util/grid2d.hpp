/*

                            +–––––––––––––––––––––––––––––––––+
                            |           grid2d.hpp            |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+
                            This is a header for creating 2D grids.
                            More specifically, we implement here a
                            Grid2D struct that can be used to store
                            a bi-dimensional grid of doubles with
                            convenient accessors. This is mainly used 
                            for price surfaces (returning a matrix of prices
                            corresponding to a grid of parameters).
*/

#ifndef grid2d_hpp
#define grid2d_hpp

#include <vector>

namespace
yvan
{
    namespace util
    {
        template<typename T>
        struct Grid2D
        {
            std::vector<T> data; // row-major
            std::size_t nrows{};
            std::size_t ncols{};

            // basic constructor
            Grid2D() = default;
            Grid2D(std::size_t rows, std::size_t cols) :
                data(rows * cols), nrows(rows), ncols(cols) { }

            // convenience accessor
            const T& operator()(std::size_t i, std::size_t j) const
            {
                return data[i * ncols + j];
            }
            T& operator()(std::size_t i, std::size_t j)
            {
                return data[i * ncols + j];
            }
        };
    }
}



#endif // grid2d_hpp