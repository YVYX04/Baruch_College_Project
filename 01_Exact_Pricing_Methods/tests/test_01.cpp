/*
test_01.cpp
Copyright © 2025 Yvan Richard

This is a test file for the Exact Pricing Methods module.
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
    std::cout << "Test 01: Exact Pricing Methods Module\n";
    std::cout << "-------------------------------------\n";

    // Create an OptionParams instance with default values (batch 1)
    yvan::option::OptionParams params;

    // Create a EuropeanOption instance
    yvan::option::EuropeanOption euro_option(params);

    // Create a BSEngine instance
    yvan::engine::BSEngine bs_engine;

    // Price the option using the BSEngine
    double option_price = bs_engine.price(euro_option.get_params());

    // Output the result
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "European Call Option Price: $" << option_price << "\n";

    return 0;
}
