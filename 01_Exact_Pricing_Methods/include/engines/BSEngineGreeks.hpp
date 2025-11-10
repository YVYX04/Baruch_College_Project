/*
                            +–––––––––––––––––––––––––––––––––+
                            |      BSEngineGreeks Class       |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This header defines the interface for
                            computing the Greeks of options priced
                            using the Black-Scholes model.
*/

#ifndef BSEngineGreeks_hpp
#define BSEngineGreeks_hpp

#include "../options/Option.hpp"
#include "IGreeks.hpp"

namespace yvan
{
    namespace engine
    {
        // Black and Scholes Greeks Engine
        class BSEngineGreeks : public IGreeks
        {
        private:
            // --- Internal Helpers ---
            // d1 and d2 calculations
            double d1(const option::OptionParams& p) const;
            double d2(const option::OptionParams& p) const;

        public:
            // --- Constructor & Destructor ---
            BSEngineGreeks() = default;
            virtual ~BSEngineGreeks() = default;

            // --- Greeks Implementations ---
            using IGreeks::delta; // bring base class overloads into scope
            using IGreeks::gamma; // bring base class overloads into scope
            
            // --- delta()
            double delta(const option::OptionParams& p) const override;

            // --- gamma()
            double gamma(const option::OptionParams& p) const override;
        };
    }
}


#endif // BSEngineGreeks_hpp