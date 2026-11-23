#include <math.h>

extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }
 
int main(void)
{
    __VERIFIER_assert(isunordered(NAN, 1.0));
    __VERIFIER_assert(isunordered(1.0, NAN));
    __VERIFIER_assert(isunordered(NAN, NAN));
    __VERIFIER_assert(!isunordered(1.0, 0.0));
 
    return 0;
}
// Imported from ESBMC regression/floats-regression/isunordered
// Needs: --interval-analysis
// RUN: --floatbv --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
