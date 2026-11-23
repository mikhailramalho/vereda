#include <assert.h>

int main()
{
   int x = 0;
   while (__VERIFIER_nondet_int())
   {
      if (!x)
      {
         assert(x == 0);
         x = 1;
      }
      else if (x == 1)
      {
         assert(x > 0);
         x = 2;
      }
      else if (x == 2)
      {
         assert(x >= 2);
         x = 3;
      }
   }
   assert(x == 3);
}
// Imported from ESBMC regression/goto-coverage/github_1337_2
// Needs: --assertion-coverage
// RUN: --assertion-coverage --unwind 3
// MODE: FUTURE
// EXPECT: ^Total Asserts: 4
// EXPECT: ^Total Assertion Instances: 4
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// Vereda: verdict none
