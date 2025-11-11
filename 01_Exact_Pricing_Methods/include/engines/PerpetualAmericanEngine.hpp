/*
                            +–––––––––––––––––––––––––––––––––+
                            |    PerpetualAmericanEngine      |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is an engine for computing
                            the price of a perpetual American option
                            based on the closed-form formula.
*/

#ifndef PerpetualAmericanEngine_hpp
#define PerpetualAmericanEngine_hpp

#include "../options/Option.hpp"
#include "IPricer.hpp"

namespace yvan
{
    namespace engine
    {
        // Perpetual American Option Engine
        class PerpetualAmericanEngine : public IPricer
        {
        private:
            // --- Internal Helpers ---
            // a1 and a2 calculations (roughly analogous to y1 and y2 but different)
            double a1(const option::OptionParams& p) const;
            double a2(const option::OptionParams& p) const;

        public:
            // --- Constructor & Destructor ---
            PerpetualAmericanEngine() = default;
            virtual ~PerpetualAmericanEngine() = default;

            // --- Price ---
            using IPricer::price; // bring base class overloads into scope
            // price function according to perpetual American option formula
            double price(const option::OptionParams& params) const override;
        };
    }
}



#endif // PerpetualAmericanEngine_hpp