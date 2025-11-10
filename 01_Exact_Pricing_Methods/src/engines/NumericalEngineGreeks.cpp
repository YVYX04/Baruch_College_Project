/*
                            +–––––––––––––––––––––––––––––––––+
                            |      NumericalEngineGreeks      |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This cpp file implements the engine for
                            computing the Greeks of options priced
                            using numerical methods (finite differences).
*/

#include "../../include/engines/NumericalEngineGreeks.hpp"
#include "../../include/engines/IPricer.hpp"
#include "../../include/engines/BSEngine.hpp"

namespace yvan
{
    namespace engine
    {
        double NumericalEngineGreeks::delta(const option::OptionParams& p) const
        {
            // We build to configs from the base by modifying the asset price
            auto p_minus = p; // copy
            p_minus.asset_price -= h_;
            auto p_plus = p; // copy
            p_plus.asset_price += h_;

            // compute price difference with the BSEngine
            double V_plus = pricer_.price(p_plus);
            double V_minus = pricer_.price(p_minus);

            // return numerical greek
            return (V_plus - V_minus) / (2.0 * h_);
        }

        double NumericalEngineGreeks::gamma(const option::OptionParams& p) const
        {
            // edit the two configs
            auto p_plus = p; p_plus.asset_price += h_;
            auto p_minus = p; p_minus.asset_price -= h_;

            // compute price difference with the BSEngine
            double V_plus = pricer_.price(p_plus);
            double V_mid  = pricer_.price(p);
            double V_minus = pricer_.price(p_minus);

            // return numerical greek
            return (V_plus - 2.0 * V_mid + V_minus) / (h_ * h_);
        }
    }
}