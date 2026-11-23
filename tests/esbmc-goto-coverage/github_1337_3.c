#include <assert.h>

void loop1()
{
   for (int i = 0; i < 2; i++)
   {
      assert(0);
   }
}

void loop2()
{
   for (int i = 0; i < 4; i++)
   {
      assert(1);
   }
}

int main()

{
   switch (__VERIFIER_nondet_int())
   {
   case 1:
      loop1();
      break;
   case 2:
      loop2();
      break;
   default:
       ;
   }
   return 0;
}
// Imported from ESBMC regression/goto-coverage/github_1337_3
// Needs: --assertion-coverage
// RUN: --unwind 1 --no-unwinding-assertions --assertion-coverage
// MODE: FUTURE
// EXPECT: ^Total Asserts: 2
// EXPECT: ^Reached Assertion Instances: 6
// EXPECT: ^Total Assertion Instances: 6
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
