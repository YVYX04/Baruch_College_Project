/*
                            +–––––––––––––––––––––––––––––––––+
                            |           IGreeks Class         |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is an interface for all
                            the greeks engines of this project.
                            As such, it contains only pure virtual
                            functions that must be overridden by
                            derived classes. Impossible to instantiate
                            directly.
*/


#ifndef IGreeks_hpp
#define IGreeks_hpp

#include <vector>
#include "../options/Option.hpp"
#include "../util/distributions.hpp"
#include "../util/grid2d.hpp"
#include "../util/param_grid.hpp"


namespace yvan
{
    namespace engine
    {
        class IGreeks
        {
        public:
            
            // --- Constructor & Destructor ---
            IGreeks() = default;
            virtual ~IGreeks() = default;

            // --- Greeks (pure virtuals) ---
            // --- delta()
            // basic 
            virtual double delta(const option::OptionParams& params) const = 0;

            // overload for sweep_1d(.)
            virtual std::vector<double>
            delta(const std::vector<option::OptionParams>& bacthes) const
            {
                // init vector
                std::vector<double> out;
                out.reserve(bacthes.size());

                // iterate over batches
                for (const auto& bacth : bacthes)
                {
                    // compute the result for the single config
                    double delta_i = delta(bacth);

                    // store the result
                    out.push_back(delta_i);
                }

                // output the entire vector
                return out;
            }

            // overload for sweep_2d(.)
            virtual util::Grid2D<double>
            delta(const util::Grid2D<option::OptionParams>& grid)
            {
                // create the grid (init to 0.0)
                util::Grid2D<double> out(grid.nrows, grid.ncols);

                // iterate over the configs and fill in out
                for (size_t i = 0; i < grid.nrows; ++i)
                {
                    for (size_t j = 0; j < grid.ncols; ++j)
                    {
                        out(i, j) = delta(grid(i, j));
                    }
                }

                // return 
                return out;
            }

            // --- gamma
            // basic 
            virtual double gamma(const option::OptionParams& params) const = 0;

            // overload for sweep_1d(.)
            virtual std::vector<double>
            gamma(const std::vector<option::OptionParams>& bacthes) const
            {
                // init vector
                std::vector<double> out;
                out.reserve(bacthes.size());

                // iterate over batches
                for (const auto& bacth : bacthes)
                {
                    // compute the result for the single config
                    double gamma_i = gamma(bacth);

                    // store the result
                    out.push_back(gamma_i);
                }

                // output the entire vector
                return out;
            }

            // overload for sweep_2d(.)
            virtual util::Grid2D<double>
            gamma(const util::Grid2D<option::OptionParams>& grid)
            {
                // create the grid (init to 0.0)
                util::Grid2D<double> out(grid.nrows, grid.ncols);

                // iterate over the configs and fill in out
                for (size_t i = 0; i < grid.nrows; ++i)
                {
                    for (size_t j = 0; j < grid.ncols; ++j)
                    {
                        out(i, j) = gamma(grid(i, j));
                    }
                }

                // return 
                return out;
            }


    
        };
    }
}

#endif // IGreeks_hpp
