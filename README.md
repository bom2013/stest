# stest
Simple unit test framework for C++, inspired by [GoogleTest](https://github.com/google/googletest).  
Thanks to [simpletest](https://github.com/kudaba/simpletest) for showing me that it's not that complicated to implement :)
## How to use
### Quick introduction
The idea is very simple, define a sequence of `tests` consisting of several` asserts`
### Install
1. Download `stest.cpp` and `stest.h`
2. Include `stest.h`
3. Use it :)
### Define test
```c++
DEFINE_TEST(<testName>)
{
  <body of the test>
}
```
`testName` should be a unique name that represents the role of the test, use of same test name twice will cause error.  
`body of the test` should use only stest asserts! do not try to use custom return, it won't work.
### asserts
| Assert | Explanation | Remarks |
| --- | --- | --- |
| `FAIL()` | Always fail |
| `ASSERT_EQ(a, b)` | Assert that a == b | Don't use this for string compare, use `ASSERT_STR_EQ` instead |
| `ASSERT_NEQ(a, b)` | Assert that a != b | Don't use this for string compare, use `ASSERT_STR_NEQ` instead |
| `ASSERT_TRUE(a)` | Assert that a is true |
| `ASSERT_FALSE(a)` | Assert that a is false |
| `ASSERT(a)` | Same as `ASSERT_TRUE(a)` |
| `ASSERT_G(a, b)` | Assert that a > b |
| `ASSERT_GE(a, b)` | Assert that a >= b |
| `ASSERT_L(a, b)` | Assert that a < b |
| `ASSERT_LE(a, b)` | Assert that a <= b |
| `ASSERT_STR_EQ(a, b)` | Assert that string a is equal to string b | For string compare use this and not `ASSERT_EQ` |
| `ASSERT_STR_NEQ(a, b)` | Assert that string a is not equal to string b | For string compare use this and not `ASSERT_NEQ` |
| `ASSERT_NEAR(a, b, eps)` | Make sure that the distance between a and b is not greater than eps |
| `ASSERT_EXCEPTION(code, exceptionType)` | Assert that exceptionType exception is thrown when code is run | The assert will fail if code is not throw any exception and also if it throw an exception of a different type. to assert that exception from any type is thrown use `ASSERT_EXCEPTION` |
| ASSERT_ANY_EXCEPTION(code)` | Assert that exception(from any type) is thrown when code is run |
| ASSERT_NO_EXCEPTION(code)` | Assert that no exception is thrown when code is run |
### Run tests
All you need to do to run the the tests is:
```c++
int main(int argc, char** argv)
{
  RUN_ALL_TESTS();
}
```
### Command line arguments
To support command line arguments use `STEST_PARSE_ARGS(argc, argv);`.  
Currently supported arguments:
| Argumets | Explanation |
| -------- | ----------- |
| `-v` \ `--verbose` | Will add to the program output a rich and detailed explanation of which assert failed and why |
### Example
Example of full use of the framework:
```c++
#include <stdexcept>
#include "stest.h"

double divide(double Numerator, double Denominator)
{
    if (Denominator == 0)
        throw std::invalid_argument("Denominator can't be zero");
    return Numerator / Denominator;
}

DEFINE_TEST(testDiv)
{
    // Normal divide
    ASSERT_EQ(divide(10, 2), 5);

    // Result less than 1
    ASSERT_EQ(divide(5, 4), 1.25);

    // Numerator less than 1
    ASSERT_EQ(divide(0.2, 4), 0.05);

    // Denominator less than 1
    ASSERT_EQ(divide(20, 0.3), 66.6666667);

    // Denominator equal 0
    ASSERT_EXCEPTION(divide(5, 0), std::invalid_argument);
}

int main(int argc, char **argv)
{
    STEST_PARSE_ARGS(argc, argv);
    RUN_ALL_TESTS();
}
```
Lets compile and run this code:
```shell
bom2013@bom2013:~$ g++ main.cpp -o main.out
bom2013@bom2013:~$ ./main.out -v
Run 1 tests
[ 100.00% ] Run 'testDiv'... Fail!
------------------------------
Fail! Not all tests pass (1 test fails(0.00% success rate))

------------------------------
In test 'testDiv':
        23: ASSERT_EQ(divide(20, 0.3), 66.6666667): '66.666667' != '66.666667'
```
Such an error is actually quite common when using floating point variables, the solution is to use `ASSERT_NEAR`.  
After change `ASSERT_EQ(divide(20, 0.3), 66.6666667);` to `ASSERT_NEAR(divide(20, 0.3), 66.6666667, 0.000001);`:
```shell
bom2013@bom2013:~$ ./main.out -v
Run 1 tests
[ 100.00% ] Run 'testDiv'... done
------------------------------
All tests pass
