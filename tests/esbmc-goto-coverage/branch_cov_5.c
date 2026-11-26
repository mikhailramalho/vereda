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
// Imported from ESBMC regression/goto-coverage/branch_cov_5
// Needs: --branch-function-coverage --k-induction
// RUN: --branch-function-coverage --k-induction
// EXPECT: ^Function Entry Points & Branches : 11
// EXPECT: ^Reached : 11
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// MODE: FUTURE
// Vereda: verdict none
