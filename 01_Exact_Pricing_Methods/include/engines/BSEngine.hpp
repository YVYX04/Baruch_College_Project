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

#ifndef BSEngine_hpp
#define BSEngine_hpp

#include "../options/Option.hpp"
#include "../options/EuropeanOption.hpp"
#include "IPricer.hpp"

namespace yvan
{
    namespace engine
    {
        // Black and Scholes Engine
        class BSEngine : public IPricer
        {
            // --- Internal Helpers ---
            // d1 and d2 calculations
            double d1(const option::OptionParams& params) const;
            double d2(const option::OptionParams& params) const;

        public:
            // --- Constructor & Destructor ---
            BSEngine() = default;
            virtual ~BSEngine() = default;

            // --- Price ---
            using IPricer::price; // bring base class overloads into scope
            // price function according to provided Black and Scholes model
            double price(const option::OptionParams& params) const override;

        };
    }
}



#endif // BSEngine_hpp