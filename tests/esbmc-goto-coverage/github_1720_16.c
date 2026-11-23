#include <stdbool.h>

int a[20];

bool foo(int il)
{
    return a[1];
}

int main()
{
    int il;
    for(il=0; foo(il) && il  < 10; ++il){}
    //if(1 && 2 && 3 && 4);
}
// Imported from ESBMC regression/goto-coverage/github_1720_16
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --unwind 10 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  4
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^Total Conditions:  6
// EXPECT: ^Condition Properties - SATISFIED:  2
// EXPECT: ^Condition Properties - UNSATISFIED:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
