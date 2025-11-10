/*
                            +–––––––––––––––––––––––––––––––––+
                            |           param_grid.cpp        |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This is an implementation for creating parameter grids.
                            More specifically, we implement here to core
                            functions:
                                - sweep_1d
                                - sweep_2d
                            This function will take as input a base param
                            configuration OptionParam and based on this will
                            generate a vector or a matrix of parameters that
                            can then be used to compute price line or price
                            surface.
*/

#include "../../include/util/param_grid.hpp"
#include "../../include/util/mesh.hpp"
#include <stdexcept>


namespace yvan
{
    namespace util
    {
        // sweep_1d(.)
        std::vector<OptionParams>
        sweep_1d(const OptionParams& base,
                double OptionParams::* field, // pointer to member
                double start, double end, double step)
        {
            // Validate inputs
            if (step <= 0.0)
            {
                throw std::invalid_argument("Step size must be positive.");
            }
            if (end < start)
            {
                throw std::invalid_argument("End value must be greater than or equal to start value.");
            }

            std::vector<OptionParams> grid;
            for (double val = start; val <= end; val += step)
            {
                OptionParams params = base; // copy base
                params.*field = val; // pointer to member syntax 
                grid.push_back(params);
            }

            // Ensure the end value is included
            if (grid.back().*field < end) // .back() gives last element
            {
                OptionParams params = base; // copy base
                params.*field = end;
                grid.push_back(params);
            }

            return grid;
        }

        // sweep_2d(.)
        util::Grid2D<OptionParams>
        sweep_2d(const OptionParams& base,
                double OptionParams::* field_x,
                double start_x, double end_x, double step_x,
                double OptionParams::* field_y,
                double start_y, double end_y, double step_y)
        {
            // Validate inputs
            if (step_x <= 0.0 || step_y <= 0.0)
            {
                throw std::invalid_argument("Step sizes must be positive.");
            }
            if (end_x < start_x)
            {
                throw std::invalid_argument("End x value must be greater than or equal to start x value.");
            }
            if (end_y < start_y)
            {
                throw std::invalid_argument("End y value must be greater than or equal to start y value.");
            }
            // Generate mesh vectors for both dimensions
            std::vector<double> mesh_x = util::mesh_vector(start_x, end_x, step_x);
            std::vector<double> mesh_y = util::mesh_vector(start_y, end_y, step_y);

            // Prepare Grid2D
            util::Grid2D<OptionParams> grid; // use default constructor for not dflt construct of OptionParams
            grid.nrows = mesh_x.size();
            grid.ncols = mesh_y.size();
            grid.data.reserve(grid.nrows * grid.ncols); // reserve space
            // Fill the grid
            for (std::size_t i = 0; i < grid.nrows; ++i)
            {
                for (std::size_t j = 0; j < grid.ncols; ++j)
                {
                    OptionParams params = base; // copy base
                    params.*field_x = mesh_x[i];
                    params.*field_y = mesh_y[j];
                    grid.data.push_back(params);
                }
            }

            return grid; // easy access via our accessors
        }
    }
}