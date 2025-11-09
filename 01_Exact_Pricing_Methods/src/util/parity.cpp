/*
                            +–––––––––––––––––––––––––––––––––+
                            |           parity.cpp            |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This object is a utility source file for
                            functions exploiting the call put parity.
*/


#include "../../include/util/parity.hpp"
#include <cmath>
#include <stdexcept>

namespace yvan
{
    namespace util
    {
        // put_from_call(): compute the put price from the call price using put-call parity
        double put_from_call(double C, const option::OptionParams& params)
        {
            // check that option type is Call
            if (params.option_type != option::OptionType::Call)
            {
                throw std::invalid_argument("OptionParams must correspond to a Call option.");
            }

            // Put-Call Parity: C + K * e^(-rT) = P + S
            double df_r = std::exp(-params.r * params.exercise_time);
            double P = C + params.strike_price * df_r - params.asset_price;
            return P;
        }
        double put_from_call(double C, const option::EuropeanOption& option)
        {
            // retrieve params (light struct thus we afford to copy)
            option::OptionParams params = option.get_params();

            // return (inner function checks option type)
            return put_from_call(C, params);
        }

        // call_from_put(): compute the call price from the put price using put-call parity
        double call_from_put(double P, const option::OptionParams& params)
        {
            // check that option type is Put
            if (params.option_type != option::OptionType::Put)
            {
                throw std::invalid_argument("OptionParams must correspond to a Put option.");
            }

            // Put-Call Parity: C + K * e^(-rT) = P + S
            double df_r = std::exp(-params.r * params.exercise_time);
            double C = P + params.asset_price - params.strike_price * df_r;
            return C;
        }
        double call_from_put(double P, const option::EuropeanOption& option)
        {
            // retrieve params (light struct thus we afford to copy)
            option::OptionParams params = option.get_params();

            // return
            return call_from_put(P, params);
        }

        // check_parity(): check if put-call parity holds for given call and put prices
        bool check_parity(double C, double P, const option::OptionParams& params, double tol)
        {
            double df_r = std::exp(-params.r * params.exercise_time);
            double lhs = C + params.strike_price * df_r;
            double rhs = P + params.asset_price;
            return std::fabs(lhs - rhs) < tol;
        }
        bool check_parity(double C, double P, const option::EuropeanOption& option, double tol)
        {
            // retrieve params (light struct thus we afford to copy)
            option::OptionParams params = option.get_params();

            // return
            return check_parity(C, P, params, tol);
        }
    }
}