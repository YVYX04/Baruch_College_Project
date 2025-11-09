/*
                            +–––––––––––––––––––––––––––––––––+
                            |        distributions.cpp        |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is a utility source file for
                            common distribution functions used in
                            the Black-Scholes engine.
*/

#include "../../include/util/distributions.hpp"
#include <boost/math/distributions/normal.hpp>

namespace yvan
{
    namespace util
    {
        // Standard normal cumulative distribution function
        double N(double x)
        {
            static boost::math::normal_distribution<double> normal_dist(0.0, 1.0);
            return boost::math::cdf(normal_dist, x);
        }

        // Standard normal probability density function
        double n(double x)
        {
            static boost::math::normal_distribution<double> normal_dist(0.0, 1.0);
            return boost::math::pdf(normal_dist, x);
        }
    }
}
