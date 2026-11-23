#include <assert.h>

void test()
{

    int x = 1;
    for (int i = 0; i < 2; i++)
        assert(x == 1);
}

int test1()
{
    int x = 1;
    for (int i = 0; i < 2; i++)
        assert(x == 1);
}

int main()
{
    test();
    test1();
}
// Imported from ESBMC regression/goto-coverage/github_1618_5
// Needs: --assertion-coverage
// RUN: --assertion-coverage --unwind 10 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Total Asserts: 2
// EXPECT: ^Reached Assertion Instances: 4
// EXPECT: ^Total Assertion Instances: 4
// EXPECT: ^Assertion Instances Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
