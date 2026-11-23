#include <assert.h>

void loop1()
{
   for (int i = 0; i < 10; i++)
   {
      assert(1);
   }
}

void loop2()
{
   for (int i = 0; i < 2; i++)
   {
      assert(0);
   }
}

int main()

{
   switch (1)
   {
   case 1:
      loop1();
      /*Fallthrough*/
   case 2:
      loop2();
    /*Fallthrough*/
   default:
       ;
   }
   return 0;
}
// Imported from ESBMC regression/esbmc/github_2071
// Needs: --k-induction --base-k-step 12
// RUN: --k-induction --base-k-step 12
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
