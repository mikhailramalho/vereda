#include <stdlib.h>
#include <assert.h>

int *a;
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

   n = __VERIFIER_nondet_int();
   
   if (n <= 0 || n >= 10){
      n=5;
      a = (int *) malloc(n * sizeof(int));
      __VERIFIER_assume(a);
   } else {      
      assert(n>0 && n<10);
      a = (int *) malloc( n * sizeof(int));
      __VERIFIER_assume(a);
   }

   test();

   return 1;
}
// Imported from ESBMC regression/esbmc/00_memory_leak_20
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --unwind 10 --no-unwinding-assertions --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
