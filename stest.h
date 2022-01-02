#pragma once
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
        const char *what() { return (message.c_str() ? message.c_str() : "Unknowen fail result"); }

    private:
        std::string message;
    };

    class SimpleTest
    {
    public:
        SimpleTest();
        virtual void runTest() = 0;
        virtual const char *name() = 0;
        virtual int getNumberOfAssertsDone() = 0;

    protected:
        int numberOfAssertsDone = 1;
    };

    // The global things that run all the tests
    static std::vector<SimpleTest *> tests;
    void RunTests();
    void addTest(SimpleTest *);

// some help macros for internal use
#define ATTACH(a, b) a##b
#define VAR_TO_STRING(a) std::to_string(a)
#define TO_STRING(a) std::string(a)

// main macros
#define DEFINE_TEST(testName)                                           \
    class ATTACH(testName, _SimpleTestClass) : public stest::SimpleTest \
    {                                                                   \
        const char *name() override { return #testName; }               \
        int getNumberOfAssertsDone() { return numberOfAssertsDone; }            \
        void runTest() override;                                        \
    } ATTACH(testName, _SimpleTestClass_Instance);                      \
    void ATTACH(testName, _SimpleTestClass)::runTest()

#define RUN_ALL_TESTS() stest::RunTests()

// Asserts macros
#define FAIL() return stest::TestResult("Assert fail");
#define SUCCESS() return stest::TestResult();

#define ASSERT_EQ(a, b)                                                                                    \
    if ((a) != (b))                                                                                        \
    {                                                                                                      \
        throw stest::TestFailException(TO_STRING("ASSERT_EQ(") + TO_STRING(#a) + TO_STRING(", ") + TO_STRING(#b) + TO_STRING("): ") + VAR_TO_STRING(a) + " != " + VAR_TO_STRING(b)); \
    }                                                                                                      \
    this->numberOfAssertsDone++;
}

// TODO: asserts
// TODO: anonymos namespace?
// TODO: Init
