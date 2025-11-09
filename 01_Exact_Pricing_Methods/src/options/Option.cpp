/*
                            +–––––––––––––––––––––––––––––––––+
                            |         Option Class            |
                            | Copyright © 2025 Yvan Richard   |
                            +–––––––––––––––––––––––––––––––––+
*/

#include "../../include/options/Option.hpp"
#include <stdexcept>

namespace yvan
{
    namespace option
    {
        // --- Constructors ---
        // --- default
        Option::Option()
            : params_{} { validate_params(); } // default initialization
        // --- with input OptionParams
        Option::Option(const OptionParams& params)
            : params_{ params } { validate_params(); }

        // --- Protected Member Functions ---
        // validate_params(): validate the OptionParams struct
        // throws std::invalid_argument if invalid params
        void Option::validate_params() const
        {
            if (params_.asset_price < 0.0)
            {
                throw std::invalid_argument("Asset price must be non-negative.");
            }
            if (params_.strike_price < 0.0)
            {
                throw std::invalid_argument("Strike price must be non-negative.");
            }
            if (params_.volatility < 0.0)
            {
                throw std::invalid_argument("Volatility must be non-negative.");
            }
            if (params_.exercise_time < 0.0)
            {
                throw std::invalid_argument("Exercise time cannot be negative.");
            }
            if (params_.option_type != OptionType::Call &&
                params_.option_type != OptionType::Put)
            {
                throw std::invalid_argument("Invalid option type.");
            }
        }

        // --- Setters ---
        // --- switch_type(): switch the option type (Call or Put)
        void Option::switch_type() noexcept
        {
            if (params_.option_type == OptionType::Call)
            {
                params_.option_type = OptionType::Put;
            }
            else
            {
                params_.option_type = OptionType::Call;
            }
        }
    }
}