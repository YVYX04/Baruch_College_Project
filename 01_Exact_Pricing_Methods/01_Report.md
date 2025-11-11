# Exact Pricing Methods
### *Project Report and User Guide*

© 2025 Yvan Richard

---

## Table of Contents
1. [Introduction](#1-introduction)  
2. [Project Architecture](#2-project-architecture)
3. [Addressing Questions & Basic Functionalities](#3-addressing-questions--basic-functionalities)  
   3.1. [Basic Pricing Engine for European Options](#31-basic-pricing-engine-for-european-options)  
   3.2. [Put Call Parity](#32-put-call-parity)  
   3.3. [Vectorized and Grid Input](#33-vectorized-and-grid-input)  
   3.4. [Option Sensitivities: Analytical & Numerical Engines](#34-option-sensitivities-analytical--numerical-engines)  
   3.5. [The `PerpetualAmericanOption` class](#35-perpetual-american-options)  
4. [Unit Tests and `main()`](#4-unit-tests-and-main)

## 1. Introduction

This first part of the project focuses on object-oriented programming (OOP)
applied to exact pricing methods for derivatives, in particular plain European
options and perpetual American options. The objective was to design a set of
classes capable of representing options, pricing them, and computing their Greeks
($\Delta$, $\Gamma$) as well as their sensitivities to varying inputs (e.g.,
generating price surfaces). All functionalities were implemented from scratch
in the module [01_Exact_Pricing_Methods](/01_Exact_Pricing_Methods/).
The attentive reader will notice that the implementation primarily relies
on the Standard Template Library (STL) and the Boost libraries—especially
for evaluating $\phi(x)$ and $\Phi(x)$. The following section presents the
overall project architecture from a high-level perspective. Subsequently, I
will present how to use some basic functionalities of the `yvan::option`, `yvan::engine`,
and `yvan::util` libraries to address the questions of our fact sheet.

## 2. Project Architecture

Here is the skeleton of the project:

```cpp
01_Exact_Pricing_Methods/
├── include/
│   ├── options/
│   │   ├── Option.hpp
│   │   ├── EuropeanOption.hpp
│   │   └── PerpetualAmericanOption.hpp
│   ├── engines/
│   │   ├── IPricer.hpp
│   │   ├── BSEngine.hpp
│   │   ├── PerpetualAmericanEngine.hpp
|   |   ├── IGreeks.hpp
|   |   ├── BSEngineGreeks.hpp
|   |   └── NumericalEngineGreeks.hpp
│   └── utils/
│       ├── distributions.hpp
│       ├── grid2d.hpp
│       ├── mesh.hpp
│       ├── param_grid.hpp
│       └── parity.hpp
├── src/
│   ├── options/
│   │   ├── Option.cpp
│   │   ├── EuropeanOption.cpp
│   │   └── PerpetualAmericanOption.cpp
│   ├── engines/
|   |   ├── BSEngine.cpp
│   │   ├── PerpetualAmericanEngine.cpp
|   |   ├── BSEngineGreeks.cpp
|   |   └── NumericalEngineGreeks.cpp
│   └── utils/
│       ├── distributions.cpp
│       ├── mesh.cpp
│       ├── param_grid.cpp
│       └── parity.hpp
├── tests/
│   ├── support/
|   |   └── unit_tests_framework.hpp
|   ├── test_cases_01.cpp
|   └── main.cpp
```

The module is split by responsibility: `options/` holds thin data-centric classes
(Option, EuropeanOption, PerpetualAmericanOption) that encapsulate parameters, payoffs,
and validation.
`engines/` provides algorithmic plug-ins via `IPricer` (e.g., `BSEngine`
for Black–Scholes pricing of European options and `PerpetualAmericanEngine` for closed-form
perpetuals) and an orthogonal `IGreeks` hierarchy with both analytic (`BSEngineGreeks`) and
numerical (`NumericalEngineGreeks`) implementations.
`utils/` concentrates shared numerics and scaffolding (distributions for $\phi,\Phi$, `grid2d`
and `mesh` for price surfaces, `param_grid` for sweeps, `parity` for put–call checks).
`src/` contains the corresponding implementations.
`tests/` bundles a self-contained lightweight unit tests framework and then all the test cases
are written in [test_cases_01.cpp](/01_Exact_Pricing_Methods/tests/test_cases_01.cpp).

The key design choice is keeping pricing and Greeks engines separate from the option types:
it enforces the Single-Responsibility Principle (options carry state and payoff definitions; engines do calculations),
enables substitutability (swap models without touching option classes), supports orthogonal composition
(choose analytic or finite-difference Greeks for any pricer), and improves testability
(mock or isolate engines). Interfaces (IPricer, IGreeks) make the system extensible (e.g. we add a binomial engines
later) while `utils/mesh/param_grid` standardise parameter sweeps and surface generation without leaking
plumbing into domain classes. Overall, the layout keeps domain models clean, concentrates math in engines,
and provides reusable utilities for batching, grids, and validation; yielding a maintainable
and easily extendable pricing stack.

## 3. Addressing Questions & Basic Functionalities

### 3.1. Basic Pricing Engine for European Options

**The `Option`, `EuropeanOption` Classes**

In this first section, I was tasked with implementing a class for a `EuropeanOption`
and then derive the price of it (either as a call or put option). First, I built the 
`EuropeanOption` class which is derived from the abstract base class `Option`:

```cpp
class EuropeanOption : public Option
{
    // ...
}
```

Here, it is important to note that the `Option` class has a unique protected member
variable called `params_` which is a struct with default parameters (replicate
the data from given batch 01) that I defined myself:

```cpp
struct OptionParams
{
    double asset_price = 60.0;
    double strike_price = 65.0;
    double r = 0.08;
    double cost_of_carry = 0.08;
    double volatility = 0.30;
    double exercise_time = 0.25;
    // enum class
    OptionType option_type = OptionType::Call;
};
```

**Why `OptionParams` as a Member Variable?**

Embedding `OptionParams` as a dedicated member struct within each `Option` class provides
both architectural clarity and computational efficiency. Conceptually, it encapsulates all
quantitative attributes of the derivative—such as the underlying price, strike, risk-free rate,
and volatility—into a single, lightweight data container, isolating the state representation
from behavioural logic. The reader will also note that I encoded my enum class `OptionType` like:
```cpp
 enum class OptionType
{
    Call = 1,
    Put = -1
};
```
which then allow me to retrieve a sign that I use in my engines:
```cpp
yvan::option::OptionParams base_config{};
int sign = static_cast<int>(base_config.option_type)
```

From a performance standpoint, this design is crucial: pricing engines operate
directly on the OptionParams instance, which can be passed by constant reference
(`const OptionParams&`) and easily retrieved from the object with:

```cpp
// default constructed with default params
yvan::option::EuropeanOption euro_op{};

// retrieve a copy of params
yvan::option::OptionParams p = euro_op.get_params();
```
hereby avoiding expensive object copying, virtual table lookups, and other
overhead associated with polymorphic class hierarchies. Since `OptionParams`
is a plain-old-data (POD) structure, its compact, contiguous memory layout ensures
good cache locality and low-latency access to its fields, a property that becomes
particularly beneficial during large-scale batch computations or Monte Carlo simulations.
Moreover, engines can efficiently iterate over collections such as `std::vector<OptionParams>`,
enabling vectorized or parallelized evaluation without the indirection cost of
full object references. In sum, the use of a structured, self-contained `OptionParams`
member transforms the option architecture into a data-oriented design that is, in my humble opinion,
both computationally optimal and cleanly extensible. For further details, please read the code.

**The `IPricer` Interface and the `BSEngine`**

The IPricer interface defines the contract for all pricing engines within the framework,
enforcing a consistent API across analytical and numerical models. It declares a pure virtual
function:
```cpp
price(const OptionParams& params) const = 0;
```
which each concrete engine must implement according to its underlying pricing logic.
This design allows to decoupling option representation (`Option` classes) from the
computational model used to value them. The `BSEngine` is the canonical implementation
of this interface, responsible for pricing plain European options under the Black–Scholes
assumptions. It leverages the closed-form solution of the model, making use of helper
utilities such as `yvan::util::N(x)` and `n(x)` from the distributions module for the
standard normal PDF and CDF evaluations (Boost was used here). Internally, the `BSEngine`
retrieves the option parameters via a constant reference to OptionParams, computes the
intermediate quantities:


$$
d_1 = \frac{\ln(S/K) + (b + \sigma^2/2)T}{\sigma\sqrt{T}}
$$

$$
d_2 = d_1 - \sigma\sqrt{T}
$$

and then evaluates the theoretical call and put prices as:
$$
C = e^{(b - r)T}S\Phi(d_1) - Ke^{-rT}\Phi(d_2)
$$

$$
P = Ke^{-rT}\Phi(-d_2) - e^{(b - r)T}S\Phi(-d_1)
$$

The clean separation between the Option data model and the `BSEngine`
computation logic allows the engine to be reused across multiple
contexts—single pricing calls, batch evaluations, or surface generation—without
dependency on option-specific behaviour (the `IPricer` API is indeed extended
to support overload of the `price()` function that accepts vector or matrix of inputs).
This modularity not only simplifies testing and future extensions but also ensures
high performance through inlined mathematical routines and parameter passing by reference.

My `BSEngine` was thoroughly tested in [test_cases_01.cpp](/01_Exact_Pricing_Methods/tests/test_cases_01.cpp)
on all four batch data and passed all tests with a precision tolerance of $10^{-5}$.
For further details, read the section:

```cpp
// /01_Exact_Pricing_Methods/tests/test_cases_01.cpp
TEST_CASE(BSEngine_Price_Function_EuropeanOptions)
{
    // (... tests ...)
}
```

### 3.2. Put Call Parity

As the fact sheet required, I leveraged the put-call parity relationship for implementing useful
functions in the `yvan::util` libraries. These functionalities have also been tested in
[test_cases_01.cpp](/01_Exact_Pricing_Methods/tests/test_cases_01.cpp). Here is a replica of my
test case:

```cpp
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
```

### 3.3. Vectorized and Grid Input

To enable batch and surface pricing while trying to maintain decent computational efficiency,
I extended the `IPricer` interface to support vectorized and grid-based evaluations.
Beyond the base scalar interface
```cpp
price(const OptionParams& params) const = 0;
```
I implemented to overload in `IPricer`:
```cpp
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
```
Internally, all vectorized computations iterate directly over contiguous memory regions.
For example, in the 1-D batch overload, a result vector is pre-allocated with reserve(batch.size())
and populated via a simple loop that calls the scalar price() method for each parameter set.
This tight iteration avoids dynamic allocations or virtual calls inside the loop, maximizing
throughput during large-scale evaluations.

The 2-D overload relies on the lightweight struct `Grid2D<T>` defined and implemented in `util/grid2d.hpp`.
Instead of using `std::vector<std::vector<T>>`, which introduces pointer indirection and non-contiguous memory,
I implemented a flat row-major storage layout using a single `std::vector<T>` of size `nrows` $\cdot$ `ncols` and overloade
`operator()` to allow $O(1)$ access:

```cpp
template<typename T>
struct Grid2D
{
    std::vector<T> data;
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
```

Once this implementation was realized, I developed to useful functions called `sweep_1d()`,
and `sweep_2d()`:

```cpp
// sweep_1d(.)
std::vector<OptionParams>
sweep_1d(const OptionParams& base,
        double OptionParams::* field,
        double start, double end, double step);

// sweep_2d(.)
util::Grid2D<OptionParams>
sweep_2d(const OptionParams& base,
        double OptionParams::* field_x,
        double start_x, double end_x, double step_x,
        double OptionParams::* field_y,
        double start_y, double end_y, double step_y);
```

`sweep_1d` and `sweep_2d` generate contiguous parameter batches and grids without leaking engine logic.
Both take a base `OptionParams` and C++ pointer-to-member selectors (e.g., `double OptionParams::* field`)
to vary specific fields over linearly spaced ranges (see implementation and `mesh.hpp`).In my opinion this was one of the best approach in
term of modularity / efficiency ratio. The user can compute how the price of the `EuropeanOption` changes
in function of whatever param he wants and he has the opportunity to either input a vector or a matrix.
`sweep_1d` returns a `std::vector<OptionParams>` for single-axis scans; `sweep_2d` returns a row-major
`util::Grid2D<OptionParams>` for bi-dimensional sweeps, preserving uniform row/column sizes and cache-friendly
storage. This design keeps sweeps data-oriented and avoids `std::vector<std::vector<T>>` fragmentation.

Here is a replica of one of my test case where we see if we are able to generate correctly a price surface
$f(\mathbf{\sigma}, \mathbf{T}) = \mathbf{C}$:

```cpp
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
```

### 3.4. Option Sensitivities: Analytical & Numerical Engines

In this section, we extend our library `yvan::engine` to compute the greeks of a EuropeanOption
based on the know closed form solutions. I will be brief in describing the logic since it is almost 
exactly the same as with the `IPricer` API and the `BSEngine`. We have an API called `IGreeks` and
then an analytical engine `BSEngineGreeks` that computes the greek. The interesting part is that 
this time this API was also used to derive another engine:

```cpp
class NumericalEngineGreeks : public IGreeks
```

This is the method to derive sensitivities when we do not have access to the closed form solution and
rely on the approximations:

$$
\Delta ;\=;\ \frac{V(S + h) - V(S - h)}{2h}
$$

$$
\Gamma ;\=;\ \frac{V(S + h) + 2 V(S) + V(S - h)}{h^2}
$$

to compute the sensitivities. Hence, we deriving the sensitivities $\Delta$ and $\Gamma$ the
user will be able to choose which engine he wants. In my test cases, I compared the precision
of this engine against the analytical one and I found that for low values of $h$ (in my implementation
the default is $h = 0.001$), the precision is quite good. Here is a replica of one of my related
test cases:

```cpp
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
    double expected_delta = 0.5946; // from given batch results

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
```
and we see that the only (non-fatal) failure that my custom unit tests framework will catch is
```cpp
EXPECT_NEAR(delta_num, expected_delta, 1e-4);
```

when the engine is configured with $h = 5.0$.

**Warning on Derivatives Computation**

If you read my code, you will notice that I did not implemented functions for
computing $\nu$ and $\Theta$ since these were not asked in the fact sheet question
and implementing them would not add anything new to the project (just need to create
their pure virtuals in `IGreeks` and overriden implementation in the relevant engines).

### 3.5. Perpetual American Options

I will also be extremely brief in this section since no innovation with respect to
the codebase occur. I simply created a class `PerpetualAmericanOption` which is largely
the same as `EuropeanOption`. 

Then, I created a new engine which is also derived from the `IPricer` API called
`PerpetualAmericanEngine` and it computes the time independent price of a put or call
option. The architecture and design choices are the exact same as for the `BSEngine`,
only the maths change. To illustrate, here is a replica of one of my test cases:

```cpp
// Test Case 020: PerpetualAmericanEngine Price over 1D Parameter Grid
TEST_CASE(PerpetualAmericanEngine_Price_1D_Parameter_Grid)
{
    // Create an engine for Perpetual American Options
    ye::PerpetualAmericanEngine pa_engine{};

    // Create a Call Perpetual American Option
    // with given parameters
    yo::PerpetualAmericanOption pa_option{};

    // data: K = 100, sig = 0.1, r = 0.1, b = 0.02, S = 110
    pa_option.strike_price(100.0);
    pa_option.volatility(0.1);
    pa_option.r(0.1);
    pa_option.cost_of_carry(0.02);
    pa_option.asset_price(110.0);

    // Sweep asset_price from 80 to 110 with step 10
    double start = 80.0;
    double end = 110.0;
    double step = 10.0;
    std::vector<yo::OptionParams> param_grid =
        yu::sweep_1d(pa_option.get_params(), &yo::OptionParams::asset_price, start, end, step);

    // Price options over the parameter grid
    std::vector<double> prices = pa_engine.price(param_grid);

    // Expected number of prices
    ASSERT_EQ(prices.size(), param_grid.size());

    // Check that the final price matches known data
    double expected_final_price = 18.5035; // for S = 110
    ASSERT_NEAR(prices.back(), expected_final_price, 1e-5);

    // Check that prices increase with asset price
    for (size_t i = 1; i < prices.size(); ++i)
    {
        ASSERT_TRUE(prices[i] >= prices[i - 1]);
    }

    return true;
}
```

## 4. Unit Tests and `main()`

**Unit Test Framework**

Finally, to make sure that all my classes and functions worked as intended,
I created a small unit tests framework that leverage a self-register system
and custom implemented macros to make my testing procedure formalized.

If you are interested, please read the implementation code in [unit_tests_framework.hpp](/01_Exact_Pricing_Methods/tests/support/unit_tests_framework.hpp).

**Final Remark on Running the `main()`**

I largely recommend to the reader to consult all my test cases (not so much)
to get a better grasp of how my libraries work. Please, notice that my test cases are
rather minimal and did not systematically tried to detect edge cases mistakes,
overflow errors, or other miscellaneous. This would be absolutely necessary if this was
going to be production code, but this is not the case.

Finally, if you wish to run the code, you simply need to run the program `main.cpp`
that will run all the test cases. To do so, use the configuration that I wrote in
`.vscode/tasks.json`. Normally, you should see the following build:

```bash
Executing task: C/C++: g++-15 build main file 

Starting build...
/opt/homebrew/bin/g++-15 -std=c++20 -Wall -Wextra -fdiagnostics-color=always -I /opt/homebrew/opt/boost/include/ -L /opt/homebrew/opt/boost/lib/ -g ../src/**/*.cpp test_cases_01.cpp /Users/yvan/Desktop/SELF/QUANTFIN/BaruchCertif/Final_Project/01_Exact_Pricing_Methods/tests/main.cpp -o /Users/yvan/Desktop/SELF/QUANTFIN/BaruchCertif/Final_Project/01_Exact_Pricing_Methods/tests/main

Build finished successfully.
```

and the associated output:

```bash
Running test: EuropeanOption_Constructors_Getters_Setters ... PASSED
Running test: EuropeanOption_Assignment_Operators ... PASSED
Running test: EuropeanOption_Payoff_Function ... PASSED
Running test: BSEngine_Price_Function_EuropeanOptions ... PASSED
Running test: Util_Parity_Functions ... PASSED
Running test: Util_Grid2D_Basic_Functionality ... PASSED
Running test: sweep_1d_basic_functionality ... PASSED
Running test: sweep_2d_basic_functionality ... PASSED
Running test: BSEngine_Price_Function_1D_Parameter_Grid ... PASSED
Running test: BSEngine_Price_Function_2D_Parameter_Grid ... PASSED
Running test: BSEngineGreeks_Basics ... PASSED
Running test: BSEngineGreeks_Delta_Range_of_S ... PASSED
Running test: BSEngineGreeks_Delta_2D_Parameter_Grid ... PASSED
Running test: BSEngineGreeks_Gamma_2D_Parameter_Grid ... PASSED
Running test: NumericalEngineGreeks_Delta_Finite_Difference ... PASSED
Running test: Precision_with_Varying_h_in_NumericalEngineGreeks_Delta ... Assertion failed at test_cases_01.cpp:659 - Expected near equality but got inequality: |delta_num - expected_delta| > 1e-4[NON-FATAL]
PASSED
Running test: NumericalEngineGreeks_Gamma_Finite_Difference_vs_BSEngineGreeks ... PASSED
Running test: PerpetualAmericanOption_Constructors_Getters_Setters ... PASSED
Running test: PerpetualAmericanEngine_Price_Calculation ... PASSED
Running test: PerpetualAmericanEngine_Price_1D_Parameter_Grid ... PASSED
Running test: PerpetualAmericanEngine_Price_2D_Parameter_Grid ... PASSED
-----------------------------------
All test(s) PASSED (21 tests).
```

One test failed as non-fatal, totally normal since this is the one where we explored
the precision of our `NumericalEngineGreeks`.



# *The End*

**Produced and Written by Yvan Richard**





