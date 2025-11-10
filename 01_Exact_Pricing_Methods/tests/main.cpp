/*
main.cpp
Copyright © 2025 Yvan Richard

This is the main test file for the Exact Pricing Methods module.
It attempts to cover a broad range of tests to ensure the correctness
and robustness of the implemented classes and functions.
*/

#include "support/unit_tests_framework.hpp"

int main()
{
    return yvan::test::run_all_tests() ? 0 : 1; // return 0 if all tests passed, 1 otherwise
}