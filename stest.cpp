#include "stest.h"

void stest::addTest(SimpleTest* st)
{
	tests.push_back(st);
}

stest::SimpleTest::SimpleTest()
{
	stest::addTest(this);
}

void stest::RunTests()
{
	if (tests.size() > 0)
	{
		int fails = 0;
		printf("Run %zu tests\n", tests.size());
		for (size_t i = 0; i < tests.size(); i++)
		{
			SimpleTest* currentTest = tests[i];
			printf("[ %f%% ] Run \'%s\'... ", (static_cast<double>(i+1) / tests.size())*100, currentTest->name());
			if (currentTest->runTest())
			{
				printf("done\n");
			}
			else
			{
				printf("Fail!\n");
				fails++;
			}
		}
		printf("------------------------------\n");
		if (!fails)
			printf("All tests pass\n");
		else
			printf("Fail! Not all tests pass (%d test fails(%f%%))\n", fails, (static_cast<double>(fails) / tests.size())*100);
	}
	else
	{
		printf("No test to run!\n");
	}
}