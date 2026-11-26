#include <assert.h>

int main()
{

   int x[2];
   x[3] = 10;
   switch (__VERIFIER_nondet_int())
   {
   case 1:
      assert(0);
   case 2:
      assert(0);
   default:
       ;
   }
   return 0;
}
// Imported from ESBMC regression/goto-coverage/branch_cov_4
// Needs: --k-induction --branch-function-coverage
// RUN: --k-induction --branch-function-coverage
// EXPECT: ^Function Entry Points & Branches : 5
// EXPECT: ^Reached : 5
// EXPECT: ^Branch Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// MODE: FUTURE
// Vereda: verdict none
