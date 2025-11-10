/*
                            +–––––––––––––––––––––––––––––––––+
                            |      NumericalEngineGreeks      |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This header defines the engine for
                            computing the Greeks of options priced
                            using numerical methods (finite differences).
*/

#ifndef NumericalEngineGreeks_hpp
#define NumericalEngineGreeks_hpp

#include "../options/Option.hpp"
#include "IGreeks.hpp"
#include "IPricer.hpp"
#include "BSEngine.hpp"

namespace yvan
{
    namespace engine
    {
        class NumericalEngineGreeks : public IGreeks
        {
        private:
            // --- Member variables ---
            const BSEngine& pricer_;
            double h_; // size of the step

        public:
            // --- Constructor & Destructor ---
            // default h = 0.01
            NumericalEngineGreeks(const engine::BSEngine& pricer, double h = 0.01) :
                pricer_(pricer), h_(h) {}
            virtual ~NumericalEngineGreeks() = default;

            // --- Overriden Greeks ---
            // --- delta()
            double delta(const option::OptionParams& p) const override;

            // --- gamma()
            double gamma(const option::OptionParams& p) const override;

            
        };
    }
}

#endif // NumericalEngineGreeks_hpp