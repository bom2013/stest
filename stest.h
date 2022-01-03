#ifndef STEST_H
#define STEST_H

#include <vector>
#include <cstdio>
#include <string>
#include <exception>

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

    // The global things that run all the tests
    static std::vector<SimpleTest *> tests;
    void RunTests();
    void addTest(SimpleTest *);
}

// some help macros for internal use
#define ATTACH(a, b) a##b
#define VAR_TO_STRING(a) std::to_string(a)
#define TO_STRING(a) std::string(a)
#define ABS(a) ((a) > 0 ? (a) : -(a))

// main macros
#define DEFINE_TEST(testName)                                           \
    class ATTACH(testName, _SimpleTestClass) : public stest::SimpleTest \
    {                                                                   \
        const char *name() override { return #testName; }               \
        void runTest() override;                                        \
    } ATTACH(testName, _SimpleTestClass_Instance);                      \
    void ATTACH(testName, _SimpleTestClass)::runTest()

#define RUN_ALL_TESTS() stest::RunTests()

// Asserts macros
#define NO_ARGS_FAIL_EXCEPTION(assertName, message) stest::TestFailException(VAR_TO_STRING(__LINE__) + TO_STRING(": ") + assertName + TO_STRING("(): ") + TO_STRING(message))
#define ONE_ARGS_FAIL_EXCEPTION(assertName, a, message) stest::TestFailException(VAR_TO_STRING(__LINE__) + TO_STRING(": ") + assertName + TO_STRING("(") + TO_STRING(#a) + TO_STRING("): ") + TO_STRING(message))
#define OPERATOR_FAIL_EXCEPTION(operatorName, a, b, oppositeOperator) stest::TestFailException(VAR_TO_STRING(__LINE__) + TO_STRING(": ") + TO_STRING(operatorName) + TO_STRING("(") + TO_STRING(#a) + TO_STRING(", ") + TO_STRING(#b) + TO_STRING("): \'") + VAR_TO_STRING(a) + TO_STRING("\' ") + TO_STRING(oppositeOperator) + TO_STRING(" \'") + VAR_TO_STRING(b) + TO_STRING("\'"))
#define STR_OPERATOR_FAIL_EXCEPTION(operatorName, a, b, oppositeOperator) stest::TestFailException(VAR_TO_STRING(__LINE__) + TO_STRING(": ") + TO_STRING(operatorName) + TO_STRING("(") + TO_STRING(#a) + TO_STRING(", ") + TO_STRING(#b) + TO_STRING("): \'") + TO_STRING(a) + TO_STRING("\' ") + TO_STRING(oppositeOperator) + TO_STRING(" \'") + TO_STRING(b) + TO_STRING("\'"))

#define OPERATOR_ASSERT(a, b, oppositeOperator, assertName)                 \
    if ((a)oppositeOperator(b))                                             \
    {                                                                       \
        throw OPERATOR_FAIL_EXCEPTION(assertName, a, b, #oppositeOperator); \
    }

#define STR_OPERATOR_ASSERT(a, b, oppositeOperator, assertName)                 \
    if ((TO_STRING((a)))oppositeOperator(TO_STRING((b))))                       \
    {                                                                           \
        throw STR_OPERATOR_FAIL_EXCEPTION(assertName, a, b, #oppositeOperator); \
    }

// logical asserts
#define FAIL() throw stest::TestFailException("FAIL")

#define ASSERT_EQ(a, b) OPERATOR_ASSERT(a, b, !=, "ASSERT_EQ")

#define ASSERT_NEQ(a, b) OPERATOR_ASSERT(a, b, ==, "ASSERT_NEQ")

#define ASSERT_TRUE(a)                                                                        \
    if (!(a))                                                                                 \
    {                                                                                         \
        throw ONE_ARGS_FAIL_EXCEPTION("ASSERT_TRUE", a, "\'false\' isn't equal to \'true\'"); \
    }

#define ASSERT_FALSE(a)                                                                        \
    if ((a))                                                                                   \
    {                                                                                          \
        throw ONE_ARGS_FAIL_EXCEPTION("ASSERT_FALSE", a, "\'true\' isn't equal to \'false\'"); \
    }

#define ASSERT(a) ASSERT_TRUE(a)

#define ASSERT_G(a, b) OPERATOR_ASSERT(a, b, <=, "ASSERT_GREATER")

#define ASSERT_GE(a, b) OPERATOR_ASSERT(a, b, <, "ASSERT_GREATER_EQUAL")

#define ASSERT_L(a, b) OPERATOR_ASSERT(a, b, >=, "ASSERT_LESS")

#define ASSERT_LE(a, b) OPERATOR_ASSERT(a, b, >, "ASSERT_LESS_EQUAL")

// string asserts
#define ASSERT_STR_EQ(a, b) STR_OPERATOR_ASSERT(a, b, !=, "ASSERT_STR_EQ")

#define ASSERT_STR_NEQ(a, b) STR_OPERATOR_ASSERT(a, b, ==, "ASSERT_STR_NEQ")

// float point assert
#define ASSERT_NEAR(a, b, eps)                                                                                                                                                                                                                                                                                                                                                                   \
    if (ABS((a) - (b)) > (eps))                                                                                                                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                                                                                                            \
        throw stest::TestFailException(VAR_TO_STRING(__LINE__) + TO_STRING(": ") + TO_STRING("ASSERT_NEAR") + TO_STRING("(") + TO_STRING(#a) + TO_STRING(", ") + TO_STRING(#b) + TO_STRING(", ") + TO_STRING(#eps) + TO_STRING("): ABS(") + VAR_TO_STRING(a) + TO_STRING(" - ") + VAR_TO_STRING(b) + TO_STRING(") = ") + VAR_TO_STRING(ABS((a) - (b))) + TO_STRING(" > ") + VAR_TO_STRING(eps)); \
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

// TODO: anonymos namespace?
// TODO: Init
// TODO: line number
// TODO: group
// TODO: temp disable
