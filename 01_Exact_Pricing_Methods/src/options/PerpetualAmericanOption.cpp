/*
                            +–––––––––––––––––––––––––––––––––+
                            |      PerpetualAmericanOption    |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+
*/

#include "../../include/options/PerpetualAmericanOption.hpp"

namespace yvan
{
    namespace option
    {
        // --- Constructors ---
        // --- default
        PerpetualAmericanOption::PerpetualAmericanOption()
            : Option{} { } // call base class default ctor
        // --- with input OptionParams
        PerpetualAmericanOption::PerpetualAmericanOption(const OptionParams& params)
            : Option{ params } { } // call base class ctor with params
    }
}