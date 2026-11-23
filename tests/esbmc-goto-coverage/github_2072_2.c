#include <stdbool.h>

int a[20];

bool foo(int il)
{
    return a[1];
}

int main()
{
    int il;
    for (il = 0; foo(il) && il < 10; ++il)
    {
    }
    // if(1 && 2 && 3 && 4);

    for (il = 0; il < 10 && foo(il); ++il)
    {
    }
}
// Imported from ESBMC regression/goto-coverage/github_2072_2
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --unwind 10
// MODE: FUTURE
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
