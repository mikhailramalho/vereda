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
// Imported from ESBMC regression/goto-coverage/github_1337_1
// Needs: --assertion-coverage
// RUN: --unwind 2 --no-unwinding-assertions --assertion-coverage
// MODE: FUTURE
// EXPECT: ^Total Asserts: 3
// EXPECT: ^Total Assertion Instances: 3
// EXPECT: ^Assertion Instances Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
