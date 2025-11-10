/*
test_cases_01.cpp
Copyright © 2025 Yvan Richard

This is a test file for the Exact Pricing Methods module.
This file contains various test cases to validate the
functionality of the implemented classes and functions.
*/

#include <iostream>
#include <iomanip>
#include "../include/options/Option.hpp"
#include "../include/options/EuropeanOption.hpp"
#include "../include/engines/IPricer.hpp"
#include "../include/engines/BSEngine.hpp"
#include "../include/engines/IGreeks.hpp"
#include "../include/engines/BSEngineGreeks.hpp"
#include "../include/engines/NumericalEngineGreeks.hpp"
#include "../include/util/grid2d.hpp"
#include "../include/util/parity.hpp"
#include "../include/util/param_grid.hpp"
#include "../include/util/mesh.hpp"
#include "../include/util/distributions.hpp"
#include "support/unit_tests_framework.hpp"

// Using the unit test framework
namespace yt = yvan::test;

// Other useful aliases
namespace yo = yvan::option;
namespace ye = yvan::engine;
namespace yu = yvan::util;

// --- EuropeanOption Tests ---

// Test Case 001: Test EuropeanOption Constructors, Getters and Setters
TEST_CASE(EuropeanOption_Constructors_Getters_Setters)
{
    
    // Default constructor for european option
    yo::EuropeanOption euro_op_01{};

    // Check randomly some default params
    double ass_price = euro_op_01.asset_price();
    ASSERT_EQ(ass_price, 60.0);

    // Check that OptionType::Call is the default
    int call_id = static_cast<int>(euro_op_01.option_type());
    ASSERT_EQ(call_id, 1);

    // Try one numerical setter
    euro_op_01.asset_price(62.0);
    ass_price = euro_op_01.asset_price();
    ASSERT_EQ(ass_price, 62.0);

    // Copy ctor
    yo::EuropeanOption euro_opt_02{euro_op_01};
    ASSERT_EQ(euro_opt_02.asset_price(), 62.0);

    // Memory independency
    euro_op_01.asset_price(63.0);
    ASSERT_EQ(euro_opt_02.asset_price(), 62.0);

    // Move ctor
    yo::EuropeanOption euro_opt_03{std::move(euro_op_01)};
    ASSERT_EQ(euro_opt_03.asset_price(), 63.0);

    // Parameterized constructor
    yo::OptionParams config{.asset_price = 64.0};
    yo::EuropeanOption euro_opt_04{config};
    ASSERT_EQ(euro_opt_04.asset_price(), 64.0);

    // switch option type (technically a setter)
    euro_opt_04.switch_type();
    int put_id = static_cast<int>(euro_opt_04.option_type());
    ASSERT_EQ(put_id, -1);

    return true;
}

// Test Case 002: EuropeanOption Assignment Operators
TEST_CASE(EuropeanOption_Assignment_Operators)
{
    // default construct an european option and change asset price
    yo::EuropeanOption euro_opt_01;
    euro_opt_01.asset_price(64.5);

    // default construct another european option
    yo::EuropeanOption euro_opt_02;

    // classic assignment
    euro_opt_02 = euro_opt_01;
    ASSERT_EQ(euro_opt_02.asset_price(), 64.5);

    // memory independency of the classic assignment
    euro_opt_01.asset_price(65.5);
    ASSERT_EQ(euro_opt_02.asset_price(), 64.5);

    // move assignment
    yo::EuropeanOption euro_opt_03;
    euro_opt_03 = std::move(euro_opt_01);
    ASSERT_EQ(euro_opt_03.asset_price(), 65.5);

    return true;
}

// Test Case 003: EuropeanOption Payoff Function
TEST_CASE(EuropeanOption_Payoff_Function)
{
    // Create a Call European Option
    yo::OptionParams call_params{};
    call_params.asset_price = 70.0;
    call_params.strike_price = 65.0;
    call_params.option_type = yo::OptionType::Call;
    yo::EuropeanOption call_option{call_params};
    double call_payoff = call_option.payoff();
    ASSERT_EQ(call_payoff, 5.0); // Payoff should be max(0, 70 - 65) = 5

    // Create a Put European Option
    yo::OptionParams put_params{};
    put_params.asset_price = 60.0;
    put_params.strike_price = 65.0;
    put_params.option_type = yo::OptionType::Put;
    yo::EuropeanOption put_option{put_params};
    double put_payoff = put_option.payoff();
    ASSERT_EQ(put_payoff, 5.0); // Payoff should be max(0, 65 - 60) = 5 

    return true;
}

// --- BSEngine Tests with Single OptionParams ---
// Test Case 004: BSEngine Price Function for European Options
TEST_CASE(BSEngine_Price_Function_EuropeanOptions)
{
    // Create a Black-Scholes Engine
    ye::BSEngine bs_engine{};

    // Create a Call European Option
    // with default parameters (batch 01 data)
    yo::EuropeanOption call_option{};

    // Price the Call Option
    double call_price = bs_engine.price(call_option.get_params());

    // Expected price from known data
    double expected_C_batch_01 = 2.13337; // Approximate value
    ASSERT_NEAR(call_price, expected_C_batch_01, 1e-5);

    // Change to Put option
    call_option.switch_type();
    double put_price = bs_engine.price(call_option.get_params());
    double expected_P_batch_01 = 5.84628; // Approximate value
    ASSERT_NEAR(put_price, expected_P_batch_01, 1e-5);

    // --- Batch 02 Data ---
    // T = 1.0, K = 100, sig = 0.2, r = 0.0, S = 100 (then C = 7.96557, P = 7.96557).
    yo::OptionParams params_batch_02{};
    params_batch_02.exercise_time = 1.0;
    params_batch_02.strike_price = 100.0;
    params_batch_02.volatility = 0.2;
    params_batch_02.r = 0.0;
    params_batch_02.cost_of_carry = 0.0;
    params_batch_02.asset_price = 100.0;

    // Price Call
    double call_price_batch_02 = bs_engine.price(params_batch_02);
    double expected_C_batch_02 = 7.96557; // Approximate value
    ASSERT_NEAR(call_price_batch_02, expected_C_batch_02, 1e-5);

    // Switch to Put
    params_batch_02.option_type = yo::OptionType::Put;
    double put_price_batch_02 = bs_engine.price(params_batch_02);
    double expected_P_batch_02 = 7.96557; // Approximate value
    ASSERT_NEAR(put_price_batch_02, expected_P_batch_02, 1e-5);


    // --- Batch 03 Data ---
    // T = 1.0, K = 10, sig = 0.50, r = 0.12, S = 5 (C = 0.204058, P = 4.07326).

    yo::OptionParams params_batch_03{};
    params_batch_03.exercise_time = 1.0;
    params_batch_03.strike_price = 10.0;
    params_batch_03.volatility = 0.50;
    params_batch_03.r = 0.12;
    params_batch_03.cost_of_carry = 0.12;
    params_batch_03.asset_price = 5.0;
    params_batch_03.option_type = yo::OptionType::Call;

    // Price Call
    double call_price_batch_03 = bs_engine.price(params_batch_03);
    double expected_C_batch_03 = 0.204058; // Approximate value
    ASSERT_NEAR(call_price_batch_03, expected_C_batch_03, 1e-5);

    // Switch to Put
    params_batch_03.option_type = yo::OptionType::Put;
    double put_price_batch_03 = bs_engine.price(params_batch_03);
    double expected_P_batch_03 = 4.07326; // Approximate value
    ASSERT_NEAR(put_price_batch_03, expected_P_batch_03, 1e-5);

    // --- Batch 04 Data ---
    // T = 30.0, K = 100.0, sig = 0.30, r = 0.08, S = 100.0 (C = 92.17570, P = 1.24750).
    yo::OptionParams params_batch_04{};
    params_batch_04.exercise_time = 30.0;
    params_batch_04.strike_price = 100.0;
    params_batch_04.volatility = 0.30;
    params_batch_04.r = 0.08;
    params_batch_04.cost_of_carry = 0.08;
    params_batch_04.asset_price = 100.0;
    params_batch_04.option_type = yo::OptionType::Call;

    // Price Call
    double call_price_batch_04 = bs_engine.price(params_batch_04);
    double expected_C_batch_04 = 92.17570; // Approximate value
    ASSERT_NEAR(call_price_batch_04, expected_C_batch_04, 1e-5);

    // Switch to Put
    params_batch_04.option_type = yo::OptionType::Put;
    double put_price_batch_04 = bs_engine.price(params_batch_04);
    double expected_P_batch_04 = 1.24750; // Approximate value
    ASSERT_NEAR(put_price_batch_04, expected_P_batch_04, 1e-5);

    return true;

}

// --- util::parity Tests ---
// Test Case 005: Put-Call Parity Functions
TEST_CASE(Util_Parity_Functions)
{
    // Create a Black-Scholes Engine
    ye::BSEngine bs_engine{};

    // Create a Call European Option with default parameters
    yo::EuropeanOption call_option{};

    // Price the Call Option
    double call_price = bs_engine.price(call_option.get_params());

    // Compute the Put price from Call using put-from-call function
    double put_price_from_call = yu::put_from_call(call_price, call_option);

    // Price the Put Option directly
    call_option.switch_type(); // switch to Put
    double put_price_direct = bs_engine.price(call_option.get_params());

    // Expect that both Put prices are approximately equal
    EXPECT_NEAR(put_price_from_call, put_price_direct, 1e-5);

    // Now compute Call price back from Put using call-from-put function
    double call_price_from_put = yu::call_from_put(put_price_direct, call_option);

    // Expect that both Call prices are approximately equal
    EXPECT_NEAR(call_price_from_put, call_price, 1e-5);

    // Finally, check parity directly
    bool parity_holds = yu::check_parity(call_price, put_price_direct, call_option);
    ASSERT_TRUE(parity_holds);

    return true;
}

// --- util::Grid2D Tests ---
// Test Case 006: util::Grid2D Basic Functionality
TEST_CASE(Util_Grid2D_Basic_Functionality)
{
    // matrix of int
    yu::Grid2D<int> M1(2, 3); // 2 rows, 3 cols

    // fill the matrix
    int count = 1;
    for (size_t i = 0; i < 2; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            // test accessor for setting data
            M1(i, j) = count;
            ++count;
        }
    }

    // check that it was filled properly and that
    // accessor works
    ASSERT_EQ(M1(1, 2), 6);

    return true;

}


// --- util::param_grid Tests ---
// Test Case 007: util::param_grid Functionality
TEST_CASE(sweep_1d_basic_functionality)
{
    // Base OptionParams
    yo::OptionParams base_params{};

    // Sweep asset_price from 50 to 70 with step 5
    double start = 50.0;
    double end = 70.0;
    double step = 5.0;

    std::vector<yo::OptionParams> param_grid =
        yu::sweep_1d(base_params, &yo::OptionParams::asset_price, start, end, step);

    // Expected number of points: (70 - 50) / 5 + 1 = 5
    ASSERT_EQ(param_grid.size(), 5);

    // Check first and last values
    ASSERT_EQ(param_grid.front().asset_price, 50.0);
    ASSERT_EQ(param_grid.back().asset_price, 70.0);

    return true;
}

// Test Case 008: util::sweep_2d Basic Functionality
TEST_CASE(sweep_2d_basic_functionality)
{
    // Base OptionParams
    yo::OptionParams base_params{};

    // Sweep asset_price from 50 to 60 with step 5
    double start_x = 50.0;
    double end_x = 60.0;
    double step_x = 5.0;

    // Sweep strike_price from 70 to 80 with step 5
    double start_y = 70.0;
    double end_y = 80.0;
    double step_y = 5.0;

    yu::Grid2D<yo::OptionParams> param_grid =
        yu::sweep_2d(base_params,
                      &yo::OptionParams::asset_price, start_x, end_x, step_x,
                      &yo::OptionParams::strike_price, start_y, end_y, step_y);

    // Expected grid size: 3 rows (50,55,60) x 3 cols (70,75,80)
    ASSERT_EQ(param_grid.nrows, 3);
    ASSERT_EQ(param_grid.ncols, 3);

    // Check some values
    ASSERT_EQ(param_grid(0, 0).asset_price, 50.0);
    ASSERT_EQ(param_grid(0, 0).strike_price, 70.0);
    ASSERT_EQ(param_grid(2, 2).asset_price, 60.0);
    ASSERT_EQ(param_grid(2, 2).strike_price, 80.0);

    return true;
}

// --- BSEngine Tests with util::param_grid ---
// Test Case 009: BSEngine Price Function over 1D Parameter Grid
TEST_CASE(BSEngine_Price_Function_1D_Parameter_Grid)
{
    // Create a Black-Scholes Engine
    ye::BSEngine bs_engine{};

    // Base OptionParams
    yo::OptionParams base_params{};

    // Sweep asset_price from 50 to 70 with step 5
    double start = 50.0;
    double end = 70.0;
    double step = 5.0;

    std::vector<yo::OptionParams> param_grid =
        yu::sweep_1d(base_params, &yo::OptionParams::asset_price, start, end, step);

    // Price options over the parameter grid
    std::vector<double> prices = bs_engine.price(param_grid);

    // Expected number of prices
    ASSERT_EQ(prices.size(), param_grid.size());

    // Check that prices increase with asset price
    for (size_t i = 1; i < prices.size(); ++i)
    {
        ASSERT_TRUE(prices[i] >= prices[i - 1]);
    }

    return true;
}

// Test Case 010: BSEngine Price Function over 2D param grid
TEST_CASE(BSEngine_Price_Function_2D_Parameter_Grid)
{
    // Create black and scholes engine
    ye::BSEngine bs_engine;

    // Basic config for OptionParmas
    yo::OptionParams base{};

    // sweep over volatility
    // (0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5) (8 points)
    double start_sig = 0.15;
    double end_sig = 0.5;
    double step_sig = 0.05;

    // sweep over exercise time
    // (0.25, 0.5, 0.75, 1.00, 1.25, 1.5) (6 points)
    double start_exe_t = 0.25;
    double end_exe_t = 1.5; 
    double step_exe_t = 0.25;

    // generate grid
    auto sig_exe_grid = yu::sweep_2d(base,
    &yo::OptionParams::volatility,
    start_sig, end_sig, step_sig,
    &yo::OptionParams::exercise_time,
    start_exe_t, end_exe_t, step_exe_t);

    // check grid dim
    ASSERT_EQ(sig_exe_grid.nrows, 8);
    ASSERT_EQ(sig_exe_grid.ncols, 6);


    // check the first values and end values
    ASSERT_EQ(sig_exe_grid(0, 0).volatility, 0.15);
    ASSERT_EQ(sig_exe_grid(0, 0).exercise_time, 0.25);
    ASSERT_EQ(sig_exe_grid(7, 5).volatility, 0.5);
    ASSERT_EQ(sig_exe_grid(7, 5).exercise_time, 1.5);

    // use the bs engine to generate back a grid
    // of prices (surface)
    auto price_surface = bs_engine.price(sig_exe_grid);

    // The price of a call option is increasing in both
    // the volatility and exercise_time. We check that for one
    // two points
    bool smaller = price_surface(0, 0) < price_surface(7, 5);
    ASSERT_TRUE(smaller);

    return true;
    
}

// --- BSEngineGreeks Tests ---
// Test Case 011: Test Basic Functionality of the BSEngineGreeks
TEST_CASE(BSEngineGreeks_Basics)
{
    // create an instance of the engine
    ye::BSEngineGreeks bs_greeks;

    // create the OptionParams configuration
    // given in the fact sheet
    // K = 100, S = 105, T = 0.5, r = 0.1, b = 0 and sig = 0.36
    yo::OptionParams batch_01;
    batch_01.asset_price = 105.0;
    batch_01.strike_price = 100.0;
    batch_01.exercise_time = 0.5;
    batch_01.r = 0.1;
    batch_01.cost_of_carry = 0.0;
    batch_01.volatility = 0.36;

    // compute delta call
    double delta_C = bs_greeks.delta(batch_01);
    ASSERT_NEAR(delta_C, 0.5946, 1e-4);

    // switch to put
    batch_01.option_type = yo::OptionType::Put;

    // compute delta put
    double delta_P = bs_greeks.delta(batch_01);
    ASSERT_NEAR(delta_P, -0.3566, 1e-4);

    return true;

}

// Test Case 012: BSEngineGreeks coupled with sweep_1d()
TEST_CASE(BSEngineGreeks_Delta_Range_of_S)
{
    // create an instance of the engine
    ye::BSEngineGreeks bs_greeks;

    // create a base config with default params
    yo::OptionParams base{};

    // create a vector with increasing value of S := asset_price
    double start_S = 80;
    double end_S = 120;
    double step_S = 10; // [80, 90, 100, 110, 120] -> 5 data points
    auto vector_S = yu::sweep_1d(base, &yo::OptionParams::asset_price,
                                start_S, end_S, step_S);

    // values of delta
    std::vector<double> delta_vs_S = bs_greeks.delta(vector_S);

    // check size of the vector
    ASSERT_EQ(delta_vs_S.size(), 5);

    return true;
}

// Test Case 013: BSEngineGreeks Delta over 2D param grid
TEST_CASE(BSEngineGreeks_Delta_2D_Parameter_Grid)
{
    // create an instance of the engine
    ye::BSEngineGreeks bs_greeks;

    // Basic config for OptionParmas
    yo::OptionParams base{};

    // sweep over asset price S
    // (80, 90, 100, 110, 120) (5 points)
    double start_S = 80.0;
    double end_S = 120.0;
    double step_S = 10.0;

    // sweep over volatility sig
    // (0.2, 0.3, 0.4, 0.5) (4 points)
    double start_sig = 0.2;
    double end_sig = 0.5;
    double step_sig = 0.1;

    // generate grid
    auto S_sig_grid = yu::sweep_2d(base,
    &yo::OptionParams::asset_price, start_S, end_S, step_S,
    &yo::OptionParams::volatility, start_sig, end_sig, step_sig);

    // check grid dim
    ASSERT_EQ(S_sig_grid.nrows, 5);
    ASSERT_EQ(S_sig_grid.ncols, 4);

    // use the bs engine to generate back a grid
    // of delta values (surface)
    auto delta_surface = bs_greeks.delta(S_sig_grid);

    // check output grid dim
    ASSERT_EQ(delta_surface.nrows, 5);
    ASSERT_EQ(delta_surface.ncols, 4);

    return true;
}

// Test Case 014: BSEngineGreeks Gamma over 2D param grid
TEST_CASE(BSEngineGreeks_Gamma_2D_Parameter_Grid)
{
    // create an instance of the engine
    ye::BSEngineGreeks bs_greeks;

    // Basic config for OptionParmas
    yo::OptionParams base{};

    // sweep over asset price S
    // (80, 90, 100, 110, 120) (5 points)
    double start_S = 80.0;
    double end_S = 120.0;
    double step_S = 10.0;

    // sweep over volatility sig
    // (0.2, 0.3, 0.4, 0.5) (4 points)
    double start_sig = 0.2;
    double end_sig = 0.5;
    double step_sig = 0.1;

    // generate grid
    auto S_sig_grid = yu::sweep_2d(base,
    &yo::OptionParams::asset_price, start_S, end_S, step_S,
    &yo::OptionParams::volatility, start_sig, end_sig, step_sig);

    // check grid dim
    ASSERT_EQ(S_sig_grid.nrows, 5);
    ASSERT_EQ(S_sig_grid.ncols, 4);

    // use the bs engine to generate back a grid
    // of gamma values (surface)
    auto gamma_surface = bs_greeks.gamma(S_sig_grid);

    // check output grid dim
    ASSERT_EQ(gamma_surface.nrows, 5);
    ASSERT_EQ(gamma_surface.ncols, 4);

    return true;
}


// --- Tests with NumericalEngineGreeks ---
// Test Case 015: NumericalEngineGreeks Delta Finite Difference
TEST_CASE(NumericalEngineGreeks_Delta_Finite_Difference)
{
    // Create a pricing engine
    ye::BSEngine bs_engine;

    // Create an param configuration
    // based on the same batch_01 data
    yo::OptionParams params{};
    params.asset_price = 105.0;
    params.strike_price = 100.0;
    params.exercise_time = 0.5;
    params.r = 0.1;
    params.cost_of_carry = 0.0;
    params.volatility = 0.36;
    params.option_type = yo::OptionType::Call;

    // Create NumericalEngineGreeks instance
    // with default h = 0.01
    ye::NumericalEngineGreeks num_greeks{bs_engine};

    // Compute delta
    double delta_num = num_greeks.delta(params);

    // Expected delta from BSEngineGreeks
    double expected_delta = 0.5946; // from previous test case

    // Expect near with high tolerance
    EXPECT_NEAR(delta_num, expected_delta, 1e-2); // PASSED

    // Expect near with low tolerance
    EXPECT_NEAR(delta_num, expected_delta, 1e-4); // PASSED

    // compute the put delta
    params.option_type = yo::OptionType::Put;
    double delta_put_num = num_greeks.delta(params);
    double expected_delta_put = -0.3566; // from previous test case

    // Expect near with high tolerance
    EXPECT_NEAR(delta_put_num, expected_delta_put, 1e-2); // PASSED

    // Expect near with low tolerance
    EXPECT_NEAR(delta_put_num, expected_delta_put, 1e-4); // PASSED

    return true;

}

// Test Case 016: Precision_with_Varying_h_in_NumericalEngineGreeks_Delta
TEST_CASE(Precision_with_Varying_h_in_NumericalEngineGreeks_Delta)
{
    // Create a pricing engine
    ye::BSEngine bs_engine;

    // Create an param configuration
    // based on the same batch_01 data
    yo::OptionParams params{};
    params.asset_price = 105.0;
    params.strike_price = 100.0;
    params.exercise_time = 0.5;
    params.r = 0.1;
    params.cost_of_carry = 0.0;
    params.volatility = 0.36;
    params.option_type = yo::OptionType::Call;

    // Expected delta from BSEngineGreeks
    double expected_delta = 0.5946; // from previous test case

    // Test with varying h values
    std::vector<double> h_values = {5, 1, 0.5, 0.1, 0.01, 0.001, 0.0001};
    for (double h : h_values)
    {
        ye::NumericalEngineGreeks num_greeks{bs_engine, h};
        double delta_num = num_greeks.delta(params);

        // Expect near with 2 decimal places
        EXPECT_NEAR(delta_num, expected_delta, 1e-2);

        // Expect near with 4 decimal places
        // NOTE: For h = 5, this test FAILED! (non-fatal)
        EXPECT_NEAR(delta_num, expected_delta, 1e-4);
    }

    return true;
}

// Test Case 017: NumericalEngineGreeks Gamma Finite Difference vs BSEngineGreeks
TEST_CASE(NumericalEngineGreeks_Gamma_Finite_Difference_vs_BSEngineGreeks)
{
    // Create a pricing engine
    ye::BSEngine bs_engine;

    // Create an param configuration
    // based on the same batch_01 data
    yo::OptionParams params{};
    params.asset_price = 105.0;
    params.strike_price = 100.0;
    params.exercise_time = 0.5;
    params.r = 0.1;
    params.cost_of_carry = 0.0;
    params.volatility = 0.36;
    params.option_type = yo::OptionType::Call;

    // Create NumericalEngineGreeks instance
    // with default h = 0.01
    ye::NumericalEngineGreeks num_greeks{bs_engine};

    // Compute gamma using NumericalEngineGreeks
    double gamma_num = num_greeks.gamma(params);

    // Compute gamma using BSEngineGreeks
    ye::BSEngineGreeks bs_greeks;
    double gamma_bs = bs_greeks.gamma(params);

    // Expect near with high tolerance
    EXPECT_NEAR(gamma_num, gamma_bs, 1e-2); // PASSED with h = 0.01

    // Expect near with low tolerance
    EXPECT_NEAR(gamma_num, gamma_bs, 1e-4); // PASSED with h = 0.01

    return true;

}

