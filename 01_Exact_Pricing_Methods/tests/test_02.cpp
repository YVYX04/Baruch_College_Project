/*
test_02.cpp
Copyright © 2025 Yvan Richard

This is a test file for the Exact Pricing Methods module.
Specifically, it tests the parameter grid generation functions.
*/

#include <iostream>
#include <iomanip>
#include "../include/options/Option.hpp"
#include "../include/options/EuropeanOption.hpp"
#include "../include/engines/IPricer.hpp"
#include "../include/engines/BSEngine.hpp"
#include "../include/util/grid2d.hpp"
#include "../include/util/parity.hpp"
#include "../include/util/param_grid.hpp"
#include "../include/util/mesh.hpp"
#include "../include/util/distributions.hpp"

int main()
{
    std::cout << "Test 02: Parameter Grid Generation\n";
    std::cout << "----------------------------------\n";

    // Create a base OptionParams instance
    yvan::option::OptionParams base_params; // default values

    // Generate a 1D parameter sweep for volatility from 0.1 to 0.5 with step 0.1
    std::vector<yvan::option::OptionParams> vol_grid =
        yvan::util::sweep_1d(base_params, &yvan::option::OptionParams::volatility,
                             0.1, 0.5, 0.1);

    // Output the generated 1D grid
    std::cout << "1D Volatility Sweep:\n";
    for (const auto& params : vol_grid)
    {
        std::cout << "Volatility: " << params.volatility << "\n";
    }
    std::cout << "\n";

    // Generate a 2D parameter sweep for asset price and strike price
    yvan::util::Grid2D<yvan::option::OptionParams> price_grid =
        yvan::util::sweep_2d(base_params,
                             &yvan::option::OptionParams::asset_price,
                             50.0, 70.0, 5.0,
                             &yvan::option::OptionParams::strike_price,
                             60.0, 80.0, 5.0);

    // Output the generated 2D grid
    std::cout << "2D Asset Price and Strike Price Sweep:\n";
    for (std::size_t i = 0; i < price_grid.nrows; ++i)
    {
        for (std::size_t j = 0; j < price_grid.ncols; ++j)
        {
            const auto& params = price_grid(i, j);
            std::cout << "( " << params.asset_price
                      << ", " << params.strike_price << ")  ";
        }
        std::cout << "\n";
    }

    // Price the options in the 1d grid using BSEngine
    yvan::engine::BSEngine bs_engine;
    std::vector<double> prices_1d = bs_engine.price(vol_grid);
    std::cout << "\nPrices for 1D Volatility Sweep:\n";
    for (std::size_t i = 0; i < vol_grid.size(); ++i)
    {
        std::cout << "Volatility: " << vol_grid[i].volatility
                  << " -> Price: $" << std::fixed << std::setprecision(4)
                  << prices_1d[i] << "\n";
    }

    // Price the options in the 2d grid using BSEngine
    yvan::util::Grid2D<double> prices_2d = bs_engine.price(price_grid);
    std::cout << "\nPrices for 2D Asset Price and Strike Price Sweep:\n";
    for (std::size_t i = 0; i < prices_2d.nrows; ++i)
    {
        for (std::size_t j = 0; j < prices_2d.ncols; ++j)
        {
            std::cout << std::fixed << std::setprecision(4)
                      << prices_2d(i, j) << "  ";
        }
        std::cout << "\n";
    }




    return 0;
}



