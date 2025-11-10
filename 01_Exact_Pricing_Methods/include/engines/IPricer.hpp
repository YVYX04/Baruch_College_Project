/*
                            +–––––––––––––––––––––––––––––––––+
                            |           IPricer Class         |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is an interface for all
                            the pricing engines of this project.
                            As such, it contains only pure virtual
                            functions that must be overridden by
                            derived classes. Impossible to instantiate
                            directly.
*/

#ifndef IPricer_hpp
#define IPricer_hpp

#include "../options/Option.hpp"
#include "../util/grid2d.hpp"
#include <vector>

namespace yvan
{
    namespace engine
    {
        // Interface for Pricer Engines
        class IPricer
        {
        public:
            // --- Constructor & Destructor ---
            IPricer() = default;
            virtual ~IPricer() = default;

            // --- Pure Virtual Function ---
            // price(): pure virtual function to compute the price of an option
            virtual double price(const option::OptionParams& params) const = 0;

            // Overloaded price function for util::sweep_1d()
            virtual std::vector<double>
            price(const std::vector<option::OptionParams>& batch) const
            {
                // init return vector
                std::vector<double> out;
                out.reserve(batch.size());

                // fill in the vector
                for (const auto& p : batch) out.push_back(price(p));

                // return
                return out;
            }

            // Overloaded price function for util::sweep_2d()
            virtual util::Grid2D<double>
            price(const util::Grid2D<option::OptionParams>& grid) const
            {
                // init the return grid to 0.0 everywhere
                util::Grid2D<double> out(grid.nrows,  grid.ncols);

                // iterate over the configs and fill out
                for (std::size_t i = 0; i < grid.nrows; ++i)
                {
                    for (std::size_t j = 0; j < grid.ncols; ++j)
                    {
                        out(i,j) = price(grid(i,j));
                    }
                }

                // return 
                return out;
            }
        };
    }
}


#endif // IPricer_hpp