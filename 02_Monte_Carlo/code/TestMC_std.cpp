// TestMC_2.cpp
//
// Systematic Monte Carlo study for European call pricing
// using Euler–Maruyama under Black–Scholes dynamics (β_CEV = 1).
//
// Sweeps Batches 1 & 2 over grids of N (timesteps) and NSim (paths),
// and prints CSV rows with MC price, SD, SE, exact price, abs/rel error,
// and how many times the simulated path hit the origin.
//
// (C) Datasim Education BC 2008-2011  |  Adapted by Yvan Richard (2025)

#include "OptionData.hpp"
#include "../UtilitiesDJD/Geometry/Range.hpp"
#include "../UtilitiesDJD/RNG/NormalGenerator.hpp"

#include <cmath>
#include <iostream>
#include <vector>
#include <iterator>
#include <numeric>
#include <tuple>
#include <iomanip>


// --- Basic stats helpers ---
template <typename Type>
double sample_std(const std::vector<Type>& data, double r, double T)
{
    // mean
    double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

    // sum of squared differences
    double sq_sum = 0.0;
    for (const auto& value : data) {
        sq_sum += (value - mean) * (value - mean);
    }

    // final result
    return std::sqrt(sq_sum / (data.size() - 1)) * std::exp(-r * T);
}

template <typename Type>
double standard_error(const std::vector<Type>& data, double r, double T)
{
    double stddev = sample_std(data, r, T);
    return stddev / std::sqrt(data.size());
}

// -----------------------------
// DISCLAIMER: THIS PART IS NOT NECESSARY SINCE WE ALREADY HAVE THE EXACT PRICES
// --- Normal CDF and BS exact price ---
// I use this small inline function for the normal CDF
// instead of pulling in Boost or other libraries for this time.
inline double norm_cdf(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Black–Scholes European call (no dividends)
inline double bs_call_price(double S, double K, double r, double sig, double T)
{
    if (T <= 0.0 || sig <= 0.0) {
        // Degenerate case (at expiry with zero vol): intrinsic value
        return std::max(0.0, S - K);
    }
    double sqrtT = std::sqrt(T);
    double d1 = (std::log(S / K) + (r + 0.5 * sig * sig) * T) / (sig * sqrtT);
    double d2 = d1 - sig * sqrtT;
    return S * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2);
}
// -----------------------------


// --- Duffy SDE pieces ---
namespace SDEDefinition
{ // Defines drift + diffusion + data

    OptionData* data;              // The data for the option MC

    double drift(double /*t*/, double X)
    {   // Drift term: r * X
        return (data->r) * X; // r - D (D=0 here)
    }

    double diffusion(double /*t*/, double X)
    {   // Diffusion term: sigma * X^beta, with beta=1
        double betaCEV = 1.0;
        return data->sig * std::pow(X, betaCEV);
    }

    double diffusionDerivative(double /*t*/, double X)
    {   // Needed for Milstein (not used here)
        double betaCEV = 1.0;
        return 0.5 * (data->sig) * (betaCEV) * std::pow(X, 2.0 * betaCEV - 1.0);
    }
} // End of namespace

// --- MC runner ---

// Here we define a struct to hold MC statistics
// They will be returned from the MC runner function
struct MCStats
{
    double price{};     // mean of discounted payoffs
    double sd{};        // sample std of discounted payoffs
    double se{};        // sd / sqrt(NSim)
    long   hit_origin{}; // count of VNew <= 0 across all steps and paths
};

// Run one MC experiment for given N (timesteps) and NSim (paths)
MCStats run_mc(const OptionData& opt, double S0, long N, long NSim)
{
    using namespace SDEDefinition;

    Range<double> range(0.0, opt.T);
    std::vector<double> x = range.mesh(N);

    // Pre-compute step sizes
    double k = opt.T / static_cast<double>(N);
    double sqrk = std::sqrt(k);

    // RNG
    NormalGenerator* myNormal = new BoostNormal();

    std::vector<double> discounted_payoffs;
    discounted_payoffs.reserve(static_cast<size_t>(NSim));

    long hit_count = 0;

    for (long i = 0; i < NSim; ++i)
    {
        double VOld = S0;
        double VNew = S0;
        for (unsigned long idx = 1; idx < x.size(); ++idx)
        {
            double dW = myNormal->getNormal();
            VNew = VOld + (k * drift(x[idx - 1], VOld))
                        + (sqrk * diffusion(x[idx - 1], VOld) * dW);
            VOld = VNew;
            if (VNew <= 0.0) ++hit_count;
        }
        double payoff = opt.myPayOffFunction(VNew);
        double disc_payoff = std::exp(-opt.r * opt.T) * payoff;
        discounted_payoffs.push_back(disc_payoff);
    }

    delete myNormal;

    // Stats on discounted payoffs
    double mean = std::accumulate(discounted_payoffs.begin(), discounted_payoffs.end(), 0.0)
                / static_cast<double>(discounted_payoffs.size());

    double sd = sample_std(discounted_payoffs, opt.r, opt.T);
    double se = sd / std::sqrt(static_cast<double>(discounted_payoffs.size()));

    return {mean, sd, se, hit_count};
}

// --- Main: systematic study ---
int main()
{
    std::cout << "Systematic MC study (Euler–Maruyama)" << std::endl;

    // Define batches (1 and 2)
    struct Batch { const char* name; double T, K, sig, r, S; };
    std::vector<Batch> batches = {
        {"Batch1", 0.25,  65.0, 0.30, 0.08,  60.0}, // exact C* ≈ 2.13337
        {"Batch2", 1.00, 100.0, 0.20, 0.00, 100.0}  // exact C* ≈ 7.96557
    };

    // Grids of timesteps N and simulations NSim
    std::vector<long> Ns    = {50, 500, 5000};
    std::vector<long> NSims = {5'000, 50'000, 500'000};

    // CSV header (useful for the visualization I will be doing later)
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "batch,N,NSim,price,sd,se,exact,abs_err,rel_err,hit_origin" << std::endl;

    for (const auto& b : batches)
    {
        OptionData opt;
        opt.K   = b.K;
        opt.T   = b.T;
        opt.r   = b.r;
        opt.sig = b.sig;
        opt.type = 1; // Call (+1). Use -1 for put if needed.

        // Wire globals used by drift/diffusion
        SDEDefinition::data = &opt;

        // Exact price (Black–Scholes)
        double exact = bs_call_price(b.S, b.K, b.r, b.sig, b.T);

        for (long N : Ns)
        {
            for (long NSim : NSims)
            {
                MCStats st = run_mc(opt, b.S, N, NSim);
                double abs_err = std::fabs(st.price - exact);
                double rel_err = abs_err / std::fabs(exact);

                std::cout << b.name << ","
                          << N << ","
                          << NSim << ","
                          << st.price << ","
                          << st.sd << ","
                          << st.se << ","
                          << exact << ","
                          << abs_err << ","
                          << rel_err << ","
                          << st.hit_origin
                          << std::endl;
            }
        }
    }

    return 0;
}

/*
Compilation command:
g++-15 -std=c++20 \
  -I . \
  -I .. \
  -I /opt/homebrew/opt/boost/include \
  ../UtilitiesDJD/RNG/NormalGenerator.cpp \
  TestMC_2.cpp \
  -L /opt/homebrew/opt/boost/lib \
  -o TestMC_2 \
  -lboost_random

Run and capture CSV:
./TestMC_2 > results.csv
*/