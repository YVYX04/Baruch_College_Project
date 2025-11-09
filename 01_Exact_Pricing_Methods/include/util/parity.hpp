/*
                            +–––––––––––––––––––––––––––––––––+
                            |           parity.hpp           |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is a utility header for
                            functions exploiting the call put parity.
*/

#ifndef parity_hpp
#define parity_hpp

#include "../options/Option.hpp"
#include "../options/EuropeanOption.hpp"

namespace yvan
{
    namespace util
    {
        // put_from_call(): compute the put price from the call price using put-call parity
        double put_from_call(double C, const option::OptionParams& params);
        double put_from_call(double C, const option::EuropeanOption& option); // overload

        // call_from_put(): compute the call price from the put price using put-call parity
        double call_from_put(double P, const option::OptionParams& params);
        double call_from_put(double P, const option::EuropeanOption& option); // overload

        // check_parity(): check that put-call parity holds for given call and put prices
        bool check_parity(double C, double P, const option::OptionParams& params, double tol = 1e-6);
        bool check_parity(double C, double P, const option::EuropeanOption& option, double tol = 1e-6);

        
    }
}

#endif // parity_hpp
