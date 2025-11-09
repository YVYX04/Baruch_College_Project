/*
                            +–––––––––––––––––––––––––––––––––+
                            |        distributions.hpp        |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is a utility header for
                            common distribution functions used in
                            the Black-Scholes engine.
*/

#ifndef distributions_hpp
#define distributions_hpp

namespace yvan
{
    namespace util
    {
        // Standard normal cumulative distribution function
        double N(double x);

        // Standard normal probability density function
        double n(double x);
    }
}

#endif // distributions_hpp
