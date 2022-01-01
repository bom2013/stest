#pragma once
#include <vector>
#include <cstdio>

namespace stest
{
    class SimpleTest
    {
    public:
		SimpleTest();
        virtual bool runTest() = 0;
        virtual const char *name() = 0;
    };
	static std::vector<SimpleTest*> tests;
	void RunTests();
	void addTest(SimpleTest*);

// some help macros for internal use
#define ATTACH(a, b) a##b

// main macros
#define DEFINE_TEST(testName)                                           \
    class ATTACH(testName, _SimpleTestClass) : public stest::SimpleTest \
    {                                                                   \
        const char *name() override { return #testName; }               \
        bool runTest() override;                                        \
    } ATTACH(testName, _SimpleTestClass_Instance);                      \
    bool ATTACH(testName, _SimpleTestClass)::runTest()

#define RUN_ALL_TESTS() stest::RunTests()
}