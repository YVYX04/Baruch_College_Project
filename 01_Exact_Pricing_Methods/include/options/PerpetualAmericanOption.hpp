/*
                            +–––––––––––––––––––––––––––––––––+
                            |      PerpetualAmericanOption    |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+
*/

#ifndef PerpetualAmericanOption_hpp
#define PerpetualAmericanOption_hpp

#include "Option.hpp"

namespace yvan
{
    namespace option
    {
        class PerpetualAmericanOption : public Option
        {
        public:
            // --- Constructors & Destructor ---
            // --- default
            PerpetualAmericanOption();
            // --- with input OptionParams
            PerpetualAmericanOption(const OptionParams& params);
            // --- copy ctor
            PerpetualAmericanOption(const PerpetualAmericanOption& source) = default;
            // --- move ctor
            PerpetualAmericanOption(PerpetualAmericanOption&&) = default; // e.g.: PerpetualAmericanOption opt_02 = std::move(opt_01);
            // --- destructor
            virtual ~PerpetualAmericanOption() = default;
            // --- Overloaded Assignment Operators ---
            // --- classic
            PerpetualAmericanOption& operator=(const PerpetualAmericanOption& source) = default;
            // --- move
            PerpetualAmericanOption& operator=(PerpetualAmericanOption&& source) = default; // e.g.: PerpetualAmericanOption opt_03; opt_03 = std::move(opt_02);

            // --- Overridden Virtual Functions ---
            // --- style(): returns the style of the option as a string
            inline OptionStyle style() const override { return OptionStyle::PerpetualAmerican; }
        };
    }
}

#endif // PerpetualAmericanOption_hpp