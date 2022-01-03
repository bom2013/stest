#include "stest.h"
#include <utility>

void stest::addTest(SimpleTest *st)
{
	allTests.push_back(st);
}

stest::SimpleTest::SimpleTest()
{
	stest::addTest(this);
}

void stest::runTests()
{
	runTests(allTests);
}

void stest::parseArg(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "-v")
			verbose = true;
	}
}

void stest::runTests(const std::vector<SimpleTest *> &tests)
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
