/*
                            +–––––––––––––––––––––––––––––––––+
                            |      Unit Tests Framework       |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            This is a lightweight header only
                            unit tests framework for creating
                            and running unit tests in C++20.
*/

#ifndef unit_tests_framework_hpp
#define unit_tests_framework_hpp

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>


namespace yvan
{
    namespace test
    {
        // Test Case Struct
        // Represents a single test case with a name and a function to execute
        struct TestCase
        {
            std::string name;
            std::function<bool()> func; // function returning bool (pass/fail)
        };

        
        // Registered Tests with Static Storage Duration
        inline std::vector<TestCase>& registry()
        {
            static std::vector<TestCase> tests; // we want this to be static so it persists across function calls
            return tests;
        }

        // Register a Test Case
        inline void register_test(const std::string& name, std::function<bool()> func)
        {
            // registry() calls the static vector and we push back the new test case
            registry().push_back({ name, func });
        }

        // Failure Handling
        struct AssertFailure { std::string message; };
        inline void faile_assert(const char *file, int line, const std::string& msg, bool fatal)
        {
            std::cerr << "Assertion failed at " << file << ":" << line << " - " << msg <<
            (fatal ? "[FATAL]" : "[NON-FATAL]") << "\n";
            if (fatal) throw AssertFailure{ msg };
        }

        // +-------–+
        // | MACROS |
        // +------––+
        /*
        Note:
        We use do { ... } while(0) trick to ensure that the macros behave like
        regular function calls, especially when used in conditional statements.
        It allows multiple statements to be grouped together safely and avoids
        issues with dangling else clauses.
        */

        // Convert to String Literal
        // --------------------–––––

        // The old preprocessor trick to convert macro arguments to string literals
        #define YT_STRINGIFY(x) #x // YT stands for yvan::test
        #define YT_TOSTR(x) YT_STRINGIFY(x)

        // Assertion & Expectation Macros
        // --------------------––––––––––––

        // ASSERT_TRUE(cond): Assert that condition is true otherwise fail the test with fatal error
        #define ASSERT_TRUE(cond) \
        do \
        { \
            if(!(cond)) \
            { \
                yvan::test::faile_assert(__FILE__, __LINE__, "Expected true but got false: " YT_TOSTR(cond), true); \
            } \
        } while(0)

        // EXPECT_TRUE(cond): Expect that condition is true otherwise log a non-fatal error
        #define EXPECT_TRUE(cond) \
        do \
        { \
            if(!(cond)) \
            { \
                yvan::test::faile_assert(__FILE__, __LINE__, "Expected true but got false: " YT_TOSTR(cond), false); \
            } \
        } while(0)

        // ASSERT_EQ(val1, val2): Assert that val1 == val2 otherwise fail the test with fatal error
        #define ASSERT_EQ(val1, val2) \
        do \
        { \
            if((val1) != (val2)) \
            { \
                yvan::test::faile_assert(__FILE__, __LINE__, "Expected equality but got inequality: " YT_TOSTR(val1) " != " YT_TOSTR(val2), true); \
            } \
        } while(0)

        // EXPECT_EQ(val1, val2): Expect that val1 == val2 otherwise log a non-fatal error
        #define EXPECT_EQ(val1, val2) \
        do \
        { \
            if((val1) != (val2)) \
            { \
                yvan::test::faile_assert(__FILE__, __LINE__, "Expected equality but got inequality: " YT_TOSTR(val1) " != " YT_TOSTR(val2), false); \
            } \
        } while(0)

        // ASSERT_NEAR(val1, val2, tol): Assert that |val1 - val2| <= tol otherwise fail the test with fatal error
        #define ASSERT_NEAR(val1, val2, tol) \
        do \
        { \
            if(std::fabs((val1) - (val2)) > (tol)) \
            { \
                yvan::test::faile_assert(__FILE__, __LINE__, "Expected near equality but got inequality: |" YT_TOSTR(val1) " - " YT_TOSTR(val2) "| > " YT_TOSTR(tol), true); \
            } \
        } while(0)

        // EXPECT_NEAR(val1, val2, tol): Expect that |val1 - val2| <= tol otherwise log a non-fatal error
        #define EXPECT_NEAR(val1, val2, tol) \
        do \
        { \
            if(std::fabs((val1) - (val2)) > (tol)) \
            { \
                yvan::test::faile_assert(__FILE__, __LINE__, "Expected near equality but got inequality: |" YT_TOSTR(val1) " - " YT_TOSTR(val2) "| > " YT_TOSTR(tol), false); \
            } \
        } while(0)

        // Test Case Registration Macro
        // ----------------------------
        /*
        Note:

        The TEST_CASE(name) macro defines a test case function with the given name
        and automatically registers it in the test registry upon program initialization.
        This allows for easy creation and management of test cases.

        This is a crucial part of the unit test framework, enabling users to define
        test cases in a simple manner without worrying about manual registration.
        */
        #define TEST_CASE(name) \
        bool name(); /* forward declaration since we need to register before definition and we assume that the return type of user is bool!!! */ \
        struct name##_registrar /* This struct is responsible for registering the test case */ \
        { \
            name##_registrar() /* Define a function to register the test case */ \
            { \
                yvan::test::register_test(YT_STRINGIFY(name), name); \
            } \
        }; \
        static name##_registrar global_##name##_registrar; /* The purpose of this static instance is to ensure the constructor is called, registering the test case before main() starts */ \
        bool name() /* actual definition of the test case function */


        // Test Runner
        // ----------------------------
        inline bool run_all_tests()
        {
            const auto& tests = registry();
            int passed = 0;
            int failed = 0;
            for (const auto& test : tests)
            {
                std::cout << "Running test: " << test.name << " ... ";
                try
                {
                    if (test.func())
                    {
                        std::cout << "PASSED\n";
                        ++passed;
                    }
                    else
                    {
                        std::cout << "FAILED\n";
                        ++failed;
                    }
                }
                catch (const AssertFailure& e)
                {
                    std::cout << "\033[31mFAILED with exception: " << e.message << "\n\033[0m";
                    ++failed;
                }
                catch (const std::exception& e)
                {
                    std::cout << "\033[31mFAILED with unexpected exception: " << e.what() << "\n\033[0m";
                    ++failed;
                }
                catch (...)
                {
                    std::cout << "\033[31mFAILED with unknown exception.\n\033[0m";
                    ++failed;
                }
            }
            std::cout << "-----------------------------------\n";

            if (!failed)
            {
                std::cout << "\033[32mAll test(s) PASSED (" << passed << " tests).\n\033[0m";
                return true;
            }
            else
            {
                std::cout << "\033[31mNot all test(s) PASSED. Summary: " << passed << " passed, " << failed << " failed.\n\033[0m";
            }
            return false;
        }




        




    } 
}



#endif // unit_tests_framework_hpp