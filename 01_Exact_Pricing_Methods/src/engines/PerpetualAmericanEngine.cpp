/*
                            +–––––––––––––––––––––––––––––––––+
                            |    PerpetualAmericanEngine      |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is an engine for computing
                            the price of a perpetual American option
                            based on the closed-form formula.
*/

#include "../../include/engines/PerpetualAmericanEngine.hpp"
#include "../../include/options/Option.hpp"

#include <cmath>

namespace yvan
{
    namespace engine
    {
        // --- Internal Helpers ---
        // a1 and a2 calculations (roughly analogous to y1 and y2 but different)
        // a1()
        double PerpetualAmericanEngine::a1(const option::OptionParams& p) const
        {
            return 0.5 - (p.cost_of_carry / (p.volatility * p.volatility));
        }

        // a2()
        double PerpetualAmericanEngine::a2(const option::OptionParams& p) const
        {
            double sigma_sq = (p.volatility * p.volatility);
            double m1 = ((p.cost_of_carry / sigma_sq) - 0.5);
            double m2 = 2.0 * p.r / sigma_sq;
            return std::sqrt(m1 * m1 + m2);
        }

        // y1 = a1 + a2
        // y2 = a1 - a2

        // --- Price ---
        double PerpetualAmericanEngine::price(const option::OptionParams& p) const
        {
            // retrieve the sign based on option type
            int sign = static_cast<int>(p.option_type);

            // compute the appropriate y (either y1 or y2)
            double y = a1(p) + sign * a2(p);

            // compute appropriate first member
            double m1 = p.strike_price / (sign * (y - 1));

            // compute second member
            double m2 = std::pow(((y - 1) * p.asset_price) / (y * p.strike_price), y);

            // return price
            return m1 * m2;
        }


    }
}