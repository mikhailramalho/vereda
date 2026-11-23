#include <assert.h>
#include <stdlib.h>

int y()
{
    int f = rand();
    while(f)
        return 2;
}

int x()
{
    int f = rand();
    if (f)
    {
        return 1;
    }
    else if(f == 2)
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
    int a,b;
    int f = a>b;//rand();
    if (f)
    {
        f = x();
    }
    else
    {
        f = x();
    }

    while(a!=b)
    {
        for(int i = 0 ; i<10;i++)
        {

        }
        a= b;
    }

    f = 2;
}
// Imported from ESBMC regression/goto-coverage/branch_cov_2
// Needs: --branch-function-coverage
// RUN: --branch-function-coverage --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Function Entry Points & Branches : 15
// EXPECT: ^Reached : 10
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// Vereda: verdict none
