/*
                            +–––––––––––––––––––––––––––––––––+
                            |      EuropeanOption Class       |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+
*/

#ifndef EuropeanOption_hpp
#define EuropeanOption_hpp

#include "Option.hpp"

namespace yvan
{
    namespace option
    {
        class EuropeanOption : public Option
        {
        public:
            // --- Constructors & Destructor ---
            // --- default
            EuropeanOption();
            // --- with input OptionParams
            EuropeanOption(const OptionParams& params);
            // --- copy ctor
            EuropeanOption(const EuropeanOption& source) = default;
            // --- move ctor
            EuropeanOption(EuropeanOption&&) = default; // e.g.: EuropeanOption opt_02 = std::move(opt_01);
            // --- destructor
            virtual ~EuropeanOption() = default;

            // --- Overloaded Assignment Operators ---
            // --- classic
            EuropeanOption& operator=(const EuropeanOption& source) = default;
            // --- move
            EuropeanOption& operator=(EuropeanOption&& source) = default; // e.g.: EuropeanOption opt_03; opt_03 = std::move(opt_02);

            // --- Overridden Virtual Functions ---
            // --- style(): returns the style of the option as a string
            inline OptionStyle style() const override { return OptionStyle::European; }

            // --- Other Member Functions ---
            // --- payoff(): returns the option's payoff
            double payoff() const;
        };
    }
}

#endif // EuropeanOption_hpp