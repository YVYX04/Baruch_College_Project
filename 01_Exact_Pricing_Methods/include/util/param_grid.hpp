/*
                            +–––––––––––––––––––––––––––––––––+
                            |           param_grid.hpp        |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This is a header for creating parameter grids.
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

#ifndef param_grid_hpp
#define param_grid_hpp

#include <vector>
#include "../options/Option.hpp"
#include "grid2d.hpp"
#include <string>


namespace yvan
{
    namespace util
    {
        using option::OptionParams;

        // sweep_1d(.)
        std::vector<OptionParams>
        sweep_1d(const OptionParams& base,
                double OptionParams::* field,
                double start, double end, double step);

        // sweep_2d(.)
        // struct ParamGrid2D
        // This a special struct that we implement for the bi-dimensional
        // parameter sweep because it would be wrong to implement a
        // std::vector<std::vector<OptionParams>> return:
        // - Not contiguous storage in memory
        // - Does not ensure that the rows have all the same size

        // Solution
        // -> Flat storage with nrows and ncols info + accessors

        util::Grid2D<OptionParams>
        sweep_2d(const OptionParams& base,
                double OptionParams::* field_x,
                double start_x, double end_x, double step_x,
                double OptionParams::* field_y,
                double start_y, double end_y, double step_y);


    }
}



#endif // param_grid_hpp