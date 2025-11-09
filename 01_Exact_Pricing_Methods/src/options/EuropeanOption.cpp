/*
                            +–––––––––––––––––––––––––––––––––+
                            |      EuropeanOption Class       |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+
*/

#include "../../include/options/EuropeanOption.hpp"
#include <algorithm>

namespace yvan
{
    namespace option
    {
        // --- Constructors ---
        // --- default
        EuropeanOption::EuropeanOption()
            : Option{} { } // call base class default ctor
        // --- with input OptionParams
        EuropeanOption::EuropeanOption(const OptionParams& params)
            : Option{ params } { } // call base class ctor with params

        // --- Other Member Functions ---
        // --- payoff(): returns the option's payoff
        double EuropeanOption::payoff() const
        {
            // retrieve sign based on option type
            int sign = static_cast<int>(params_.option_type);
            // diff
            double diff = sign * (params_.asset_price - params_.strike_price);
            // payoff
            return std::max(0.0, diff);
        }
    }
}