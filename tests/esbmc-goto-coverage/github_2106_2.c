#include <assert.h>
#include <stdlib.h>

int y()
{
    int f = rand();
    while (f)
        return 2;
}

int x()
{
    int f = rand();
    if (f)
    {
        return 1;
    }
    else if (f == 2)
    {
        return 0;
    }
    else
    {
    }
    x();
    return 2;
}

int main()
{
    int a, b;
    int f = a > b; // rand();
    if (f)
    {
        f = x();
    }
    else
    {
        f = x();
    }

    while (a != b)
    {
        for (int i = 0; i < 10; i++)
        {
        }
        a = b;
    }

    f = 2;
}
// Imported from ESBMC regression/goto-coverage/github_2106_2
// Needs: --condition-coverage --k-induction --base-k-step 3 --no-standard-checks
// RUN: --condition-coverage --k-induction --base-k-step 3 --function y --no-standard-checks
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
