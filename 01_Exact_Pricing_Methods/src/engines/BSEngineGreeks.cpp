/*
                            +–––––––––––––––––––––––––––––––––+
                            |      BSEngineGreeks Class       |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This cpp file defines the implementation for
                            computing the Greeks of options priced
                            using the Black-Scholes model.
*/

#include "../../include/engines/BSEngineGreeks.hpp"
#include "../../include/util/distributions.hpp"
#include <cmath>

namespace yvan
{
    namespace engine
    {
        // --- Internal Helpers ---
        double BSEngineGreeks::d1(const option::OptionParams& p) const
        {
            // Compute d1 using the Black-Scholes formula
            double num = std::log(p.asset_price / p.strike_price)
                + p.exercise_time * (p.cost_of_carry + (p.volatility * p.volatility) / 2);
            double den = p.volatility * std::sqrt(p.exercise_time);
            return num / den;
        }
        double BSEngineGreeks::d2(const option::OptionParams& p) const
        {
            return BSEngineGreeks::d1(p) - p.volatility * std::sqrt(p.exercise_time);
        }
        
        // --- Greeks Implementation ---
        // --- delta()
        double BSEngineGreeks::delta(const option::OptionParams& p) const
        {
            // retrieve the option type
            int sign = static_cast<int>(p.option_type); // := 1 for Call, := -1 for Put

            // formula:
            // delta_c = e^((b-r)*T) * N(d1)
            // delta_p = -e^((b-r)*T) * N(-d1)

            // compute d1
            double D1 = d1(p);

            // return delta
            return sign * std::exp((p.cost_of_carry - p.r) * p.exercise_time) * util::N(sign * D1);
        }

        // --- gamma()
        double BSEngineGreeks::gamma(const option::OptionParams& p) const
        {
            // note: gamma is the same for both put and call
            // compute d1
            double D1 = d1(p);

            // numerator
            double num = util::n(D1) * std::exp((p.cost_of_carry - p.r) * p.exercise_time);
            double den = p.asset_price * p.volatility * std::sqrt(p.exercise_time);

            // return gamma
            return num / den;
        }
    }
}