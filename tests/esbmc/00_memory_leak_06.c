#include <stdlib.h>
#include <assert.h>

int * a;
int n;

int test(){

   int i;

   for (i = 0; i < n; ++i){
     if (a[i])
       n--;
      
     a[i] = 0;            
   }

   return 0;
}

int main(){

   n = 2;//nondet_int(); //__NONDET__();
   
   if (n <= 0 || n >= 10){
      n=5;
      a = (int *) malloc(n * sizeof(int));
      __VERIFIER_assume(a);
   } else {      
      assert(n>0 && n<10);
      a = (int *) malloc( n * sizeof(int));
      __VERIFIER_assume(a);
   }

//   __VERIFIER_assume(a);

   test();

   return 1;
}
// Imported from ESBMC regression/esbmc/00_memory_leak_06
// Needs: --memory-leak-check
// RUN: --no-slice --memory-leak-check --unwind 10 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
