/*
price_surface.cpp
-----------------
In this file, we generate price surface data for different option parameters.
This data can be used for visualization purposes.
This code uses C++20 features.

Author: Yvan Richard
Date: Fall 2025
*/

// standard library includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

// yvan library includes
// --- options ---
#include "options/Option.hpp"
#include "options/EuropeanOption.hpp"

// --- engines ---
#include "engines/IPricer.hpp"
#include "engines/BSEngine.hpp"
#include "engines/IGreeks.hpp"
#include "engines/BSEngineGreeks.hpp"

// --- utils ---
#include "util/mesh.hpp"
#include "util/grid2d.hpp"
#include "util/param_grid.hpp"
#include "util/distributions.hpp"




namespace yo = yvan::option;
namespace yu = yvan::util;
namespace ye = yvan::engine;

// generate_price_surface()
void generate_price_surface(const yo::OptionParams& p)
{
    // create parameter grids for asset price and time to maturity
    double start_S = p.asset_price * 0.5;
    double end_S = p.asset_price * 1.5;
    double step_S = (end_S - start_S) / 50.0;

    double start_T = p.exercise_time * 0.5;
    double end_T = p.exercise_time * 1.5;
    double step_T = (end_T - start_T) / 50.0;

    // grid of parameters
    yu::Grid2D<yo::OptionParams> grid = yu::sweep_2d(p,
        &yo::OptionParams::asset_price, start_S, end_S, step_S,
        &yo::OptionParams::exercise_time, start_T, end_T, step_T);

    // create BS pricer
    ye::BSEngine bs_pricer;

    // price over the grid
    yu::Grid2D<double> price_grid = bs_pricer.price(grid);

    // output to CSV
    std::ofstream file("../data/price_surface.csv"); // file is now a stream to write to

    // write header
    file << "asset_price,exercise_time,price\n";
    for (std::size_t i = 0; i < grid.nrows; ++i)
    {
        for (std::size_t j = 0; j < grid.ncols; ++j)
        {
            file << std::fixed << std::setprecision(4)
                 << grid(i,j).asset_price << ","
                 << grid(i,j).exercise_time << ","
                 << price_grid(i,j) << "\n";
        }
    }

    file.close(); // close the file stream

    return;
}

// generate_delta_surface()
void generate_delta_surface(const yo::OptionParams& p)
{
    // create parameter grids for asset price and volatility
    double start_S = p.asset_price * 0.5;
    double end_S = p.asset_price * 1.5;
    double step_S = (end_S - start_S) / 50.0;

    double start_T = p.exercise_time * 0.5;
    double end_T = p.exercise_time * 1.5;
    double step_T = (end_T - start_T) / 50.0;

    // grid of parameters
    yu::Grid2D<yo::OptionParams> grid = yu::sweep_2d(p,
        &yo::OptionParams::asset_price, start_S, end_S, step_S,
        &yo::OptionParams::exercise_time, start_T, end_T, step_T);

    // create BS Greeks engine
    ye::BSEngineGreeks bs_greeks;

    // delta over the grid
    yu::Grid2D<double> delta_grid = bs_greeks.delta(grid);

    // output to CSV
    std::ofstream file("../data/delta_surface.csv"); // file is now a stream to write to

    // write header
    file << "asset_price,exercise_time,delta\n";
    for (std::size_t i = 0; i < grid.nrows; ++i)
    {
        for (std::size_t j = 0; j < grid.ncols; ++j)
        {
            file << std::fixed << std::setprecision(4)
                 << grid(i,j).asset_price << ","
                 << grid(i,j).exercise_time << ","
                 << delta_grid(i,j) << "\n";
        }
    }

    file.close(); // close the file stream

    return;
}




// run our two functions
int main()
{
    // print welcome message
    std::cout << "Option Price and Delta Surface Generator" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "(The spot price and the time to maturity will vary)" << std::endl;
    
    // build OptionParam
    yo::OptionParams p1{};
    p1.option_type = yo::OptionType::Call;

    // ask the user some input
    using std::cout; using std::endl; using std::cin;
    double K, T, r, b, S, sigma;

    // take parameter by param
    cout << "Enter the strike price K: ";
    cin >> K; p1.strike_price = K;
    cout << "Enter the time to maturity: ";
    cin >> T; p1.exercise_time = T;
    cout << "Enter the interest rate: ";
    cin >> r; p1.r = r;
    cout << "Enter the cost of carry: ";
    cin >> b; p1.cost_of_carry = b;
    cout << "Enter the asset price: ";
    cin >> S; p1.asset_price = S;
    cout << "Enter the volatility: ";
    cin >> sigma; p1.volatility = sigma;

    // generate price surface
    generate_price_surface(p1);

    // generate delta surface
    generate_delta_surface(p1);

    return 0;
}