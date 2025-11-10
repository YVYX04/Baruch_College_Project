/*
                            +–––––––––––––––––––––––––––––––––+
                            |           BSEngine Class        |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is an engine for computing
                            the price of derivatives (namely options),
                            based on the exact formula of Black and
                            Scholes.
*/

#include "../../include/engines/BSEngine.hpp"
#include "../../include/options/EuropeanOption.hpp"
#include "../../include/util/distributions.hpp"

#include <cmath>

namespace yvan
{
    namespace engine
    {
        // --- Internal Helpers ---
        double BSEngine::d1(const option::OptionParams& p) const
        {
            // Compute d1 using the Black-Scholes formula
            double num = std::log(p.asset_price / p.strike_price)
                + p.exercise_time * (p.cost_of_carry + (p.volatility * p.volatility) / 2);
            double den = p.volatility * std::sqrt(p.exercise_time);
            return num / den;
        }
        double BSEngine::d2(const option::OptionParams& p) const
        {
            return BSEngine::d1(p) - p.volatility * std::sqrt(p.exercise_time);
        }

        // --- Price ---
        double BSEngine::price(const option::OptionParams& p) const
        {

            // Compute d1 and d2
            double D1 = d1(p);
            double D2 = d2(p);

            // Retrieve sign based on option type
            int sign = static_cast<int>(p.option_type);

            // Compute the Black-Scholes price (Call or Put)
            double df_r = std::exp(-p.r * p.exercise_time);
            double fwd_factor = std::exp((p.cost_of_carry - p.r) * p.exercise_time);
            double price = sign * ( p.asset_price * fwd_factor * util::N(sign * D1)
                                - p.strike_price * df_r * util::N(sign * D2) );

            return price;
        }
    }
}
