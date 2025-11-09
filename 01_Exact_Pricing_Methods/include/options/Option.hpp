/*
                            +–––––––––––––––––––––––––––––––––+
                            |         Option Class            |
                            | Copyright © 2025 Yvan Richard   |
                            +–––––––––––––––––––––––––––––––––+
*/



#ifndef Option_hpp
#define Option_hpp

#include <string>

namespace yvan
{
    namespace option
    {
        // Enumeration for Option Types
        enum class OptionType
        {
            Call = 1,
            Put = -1
        };

        // Enumeration for Option Style
        enum class OptionStyle
        {
            European,
            PerpetualAmerican
        };

        // Struct for Option Params
        // Default value to accomodate data from batch 1
        struct OptionParams
        {
            double asset_price = 60.0;
            double strike_price = 65.0;
            double r = 0.08;                                   // for stocks, we assume b = r
            double cost_of_carry = 0.08;
            double volatility = 0.30;
            double exercise_time = 0.25;                       // (unit in years)
            OptionType option_type = OptionType::Call;         // either Call or Put
        };

        // The Option Class (Abstract Base Class)
        class Option
        {
        protected:
            // --- Member Variables ---
            OptionParams params_;

            // --- Protected Member Functions ---
            // validate_params(): validate the OptionParams struct
            // throws std::invalid_argument if invalid params
            void validate_params() const;

        public:
            // --- Constructors & Destructor ---
            // --- default
            Option();
            // --- with input OptionParams
            Option(const OptionParams& params);
            // --- copy ctor
            Option(const Option& source) = default;
            // --- move ctor
            Option(Option&&) = default; // e.g.: Option opt_02 = std::move(opt_01);
            // --- destructor
            virtual ~Option() = default;

            // --- Overloaded Assignment Operators ---
            // --- classic
            Option& operator=(const Option& source) = default;
            // --- move
            Option& operator=(Option&& source) = default; // e.g.: Option opt_03; opt_03 = std::move(opt_02);

            // --- Getters & Setters ---
            // --- Getters
            // --- get_params(): returns the OptionParams struct by value since it's small
            inline OptionParams get_params() const noexcept { return params_; }
            inline double asset_price() const noexcept { return params_.asset_price; }
            inline double strike_price() const noexcept { return params_.strike_price; }
            inline double r() const noexcept { return params_.r; }
            inline double cost_of_carry() const noexcept { return params_.cost_of_carry; }
            inline double volatility() const noexcept { return params_.volatility; }
            inline double exercise_time() const noexcept { return params_.exercise_time; }
            inline OptionType option_type() const noexcept { return params_.option_type; }

            // --- Setters (overloaded based on input)
            // --- set_params(): sets the OptionParams struct by copying the input (no move semantics here)
            inline void set_params(const OptionParams& params) noexcept { params_ = params; }
            inline void asset_price(const double asset_price) noexcept { params_.asset_price = asset_price; }
            inline void strike_price(const double strike_price) noexcept { params_.strike_price = strike_price; }
            inline void r(const double r) noexcept { params_.r = r; }
            inline void cost_of_carry(const double cost_of_carry) noexcept { params_.cost_of_carry = cost_of_carry; }
            inline void volatility(const double volatility) noexcept { params_.volatility = volatility; }
            inline void exercise_time(const double exercise_time) noexcept { params_.exercise_time = exercise_time; }
            // --- switch_type(): switch the option type (Call or Put)
            void switch_type() noexcept;

            // --- Pure Virtual Function ---
            virtual OptionStyle style() const = 0;
        };
    }
}


# endif // Option.hpp