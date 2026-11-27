#include <stdlib.h>

int main(){

   int * a;
   int i,j;
   int k = 2;//nondet_int(); //__NONDET__();

   if ( k <= 0 ) return -1;
   
   a= malloc( k * sizeof(int));
   __VERIFIER_assume(a);

//   __VERIFIER_assume(a);
//   __VERIFIER_assume(k >= 100);
   
   for (i =0 ; i != k; i++)
      if (a[i] <= 1) break;

   i--;
   
   for (j = 0; j < i; ++j)
      a[j] = a[i];
   

   return 0;

}
// Imported from ESBMC regression/esbmc/00_memory_leak_08
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --unwind 1 --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
