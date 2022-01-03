#ifndef STEST_H
#define STEST_H

#include <vector>
#include <cstdio>
#include <string>
#include <utility>

namespace stest
{
    class TestFailException : public std::exception
    {
    public:
        TestFailException()
        {
            message = nullptr;
        }
        TestFailException(const char *message)
        {
            this->message = message;
        }
        TestFailException(std::string str)
        {
            this->message = str;
        }
        const char *what() { return (message.c_str() ? message.c_str() : "Unknown fail result"); }

    private:
        std::string message;
    };

    class SimpleTest
    {
    public:
        SimpleTest();
        virtual void runTest() = 0;
        virtual const char *name() = 0;
    };

    // global setting
    static bool verbose = false;

    // The global things that run all the tests
    static std::vector<SimpleTest *> tests;
    void addTest(SimpleTest *st) { tests.push_back(st); }
    SimpleTest::SimpleTest()
    {
        stest::addTest(this);
    }
    void parseArg(int argc, char **argv)
    {
        for (int i = 1; i < argc; i++)
        {
            if (std::string(argv[i]) == "-v")
                verbose = true;
        }
    }
    void runTests()
    {
        if (tests.size() > 0)
        {
            std::vector<std::pair<SimpleTest *, TestFailException>> fails;
            printf("Run %zu tests\n", tests.size());
            for (size_t i = 0; i < tests.size(); i++)
            {
                SimpleTest *currentTest = tests[i];
                printf("[ %.2f%% ] Run \'%s\'... ", (static_cast<double>(i + 1) / tests.size()) * 100, currentTest->name());
                try
                {
                    currentTest->runTest();
                    printf("done\n");
                }
                catch (TestFailException tfe)
                {
                    printf("Fail!\n");
                    fails.push_back(std::pair<SimpleTest *, TestFailException>(currentTest, tfe));
                }
            }
            printf("------------------------------\n");
            if (fails.empty())
                printf("All tests pass\n");
            else
                printf("Fail! Not all tests pass (%lu test fails(%.2f%% success rate))\n\n", fails.size(), 100 - ((static_cast<double>(fails.size()) / (tests.size())) * 100));
            if (verbose)
            {
                for (auto fail : fails)
                {
                    printf("------------------------------\n");
                    printf("In test \'%s\':\n\t%s\n", fail.first->name(), fail.second.what());
                }
            }
        }
        else
        {
            printf("No test to run!\n");
        }
    }

}

// some help macros for internal use
#define ABS(a) ((a) > 0 ? (a) : -(a))

// macro for main functions
#define RUN_ALL_TESTS() stest::runTests()
#define STEST_PARSE_ARGS(argc, argv) stest::parseArg(argc, argv)

// define test macro
#define DEFINE_TEST(testName)                                   \
    class testName##_SimpleTestClass : public stest::SimpleTest \
    {                                                           \
        const char *name() override { return #testName; }       \
        void runTest() override;                                \
    } testName##_SimpleTestClass_Instance;                      \
    void testName##_SimpleTestClass::runTest()

// Asserts macros
#define LINE_NUMBER_STR std::to_string(__LINE__) + ": "
#define NO_ARGS_FAIL_EXCEPTION(assertName, message) stest::TestFailException(LINE_NUMBER_STR + ": " + assertName + "(): " + message)
#define ONE_ARGS_FAIL_EXCEPTION(assertName, a, message) stest::TestFailException(LINE_NUMBER_STR + assertName + "(" + #a + "): " + message)
#define OPERATOR_FAIL_EXCEPTION(operatorName, a, b, oppositeOperator) stest::TestFailException(LINE_NUMBER_STR + operatorName + "(" + #a + ", " + #b + "): \'" + std::to_string(a) + "\' " + oppositeOperator + " \'" + std::to_string(b) + "\'")
#define STR_OPERATOR_FAIL_EXCEPTION(operatorName, a, b, oppositeOperator) stest::TestFailException(LINE_NUMBER_STR + operatorName + "(" + #a + ", " + #b + "): \'" + a + "\' " + oppositeOperator + " \'" + b + "\'")

#define OPERATOR_ASSERT(a, b, oppositeOperator, assertName)                 \
    if ((a)oppositeOperator(b))                                             \
    {                                                                       \
        throw OPERATOR_FAIL_EXCEPTION(assertName, a, b, #oppositeOperator); \
    }

#define STR_OPERATOR_ASSERT(a, b, oppositeOperator, assertName)                 \
    if ((std::string((a)))oppositeOperator(std::string((b))))                   \
    {                                                                           \
        throw STR_OPERATOR_FAIL_EXCEPTION(assertName, a, b, #oppositeOperator); \
    }

#define BOOLEAN_ASSERT(a, name, goodvalue, badvalue)                                                                        \
    if ((a) != goodvalue)                                                                                                   \
    {                                                                                                                       \
        throw ONE_ARGS_FAIL_EXCEPTION(name, a, std::string("\'") + #badvalue + "\' isn't equal to \'" + #goodvalue + "\'"); \
    }

// logical asserts
#define FAIL() throw stest::TestFailException(LINE_NUMBER_STR + "FAIL")

#define ASSERT_EQ(a, b) OPERATOR_ASSERT(a, b, !=, "ASSERT_EQ")

#define ASSERT_NEQ(a, b) OPERATOR_ASSERT(a, b, ==, "ASSERT_NEQ")

#define ASSERT_TRUE(a) BOOLEAN_ASSERT(a, "ASSERT_TRUE", true, false)

#define ASSERT_FALSE(a) BOOLEAN_ASSERT(a, "ASSERT_FALSE", false, true)

#define ASSERT(a) ASSERT_TRUE(a)

#define ASSERT_G(a, b) OPERATOR_ASSERT(a, b, <=, "ASSERT_GREATER")

#define ASSERT_GE(a, b) OPERATOR_ASSERT(a, b, <, "ASSERT_GREATER_EQUAL")

#define ASSERT_L(a, b) OPERATOR_ASSERT(a, b, >=, "ASSERT_LESS")

#define ASSERT_LE(a, b) OPERATOR_ASSERT(a, b, >, "ASSERT_LESS_EQUAL")

// string asserts
#define ASSERT_STR_EQ(a, b) STR_OPERATOR_ASSERT(a, b, !=, "ASSERT_STR_EQ")

#define ASSERT_STR_NEQ(a, b) STR_OPERATOR_ASSERT(a, b, ==, "ASSERT_STR_NEQ")

// float point assert
#define ASSERT_NEAR(a, b, eps)                                                                                                                                                                                                                    \
    if (ABS((a) - (b)) > (eps))                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                             \
        throw stest::TestFailException(LINE_NUMBER_STR + "ASSERT_NEAR" + "(" + #a + ", " + #b + ", " + #eps + "): ABS(" + std::to_string(a) + " - " + std::to_string(b) + ") = " + std::to_string(ABS((a) - (b))) + " > " + std::to_string(eps)); \
    }

// some interesting asserts
#define ASSERT_EXCEPTION(code, exceptionType)                                       \
    try                                                                             \
    {                                                                               \
        code;                                                                       \
        throw NO_ARGS_FAIL_EXCEPTION("ASSERT_EXCEPTION", "No exception throw");     \
    }                                                                               \
    catch (stest::TestFailException tfe)                                            \
    {                                                                               \
        throw tfe;                                                                  \
    }                                                                               \
    catch (exceptionType)                                                           \
    {                                                                               \
    }                                                                               \
    catch (...)                                                                     \
    {                                                                               \
        throw NO_ARGS_FAIL_EXCEPTION("ASSERT_EXCEPTION", "Other exception thrown"); \
    }

#define ASSERT_ANY_EXCEPTION(code)                                                  \
    try                                                                             \
    {                                                                               \
        code;                                                                       \
        throw NO_ARGS_FAIL_EXCEPTION("ASSERT_ANY_EXCEPTION", "No exception throw"); \
    }                                                                               \
    catch (stest::TestFailException tfe)                                            \
    {                                                                               \
        throw tfe;                                                                  \
    }                                                                               \
    catch (...)                                                                     \
    {                                                                               \
    }

#define ASSERT_NO_EXCEPTION(code)                                                       \
    try                                                                                 \
    {                                                                                   \
        code;                                                                           \
    }                                                                                   \
    catch (...)                                                                         \
    {                                                                                   \
        throw NO_ARGS_FAIL_EXCEPTION("ASSERT_NO_EXCEPTION", "An exception was thrown"); \
    }

#endif