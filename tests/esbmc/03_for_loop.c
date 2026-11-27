#include <assert.h> 
int a[2];

int foo(int x){
   assert(x<2);
   return 1;
}

int main(){
   int il;
   for(il=0; foo(il) && il  < 2; ++il)
   {};

//   for(il=0; il < 10 && foo(il) ; ++il){
//   assert(il< 11);
//     }

   return 0;
}
// Imported from ESBMC regression/esbmc/03_for_loop
// Needs: --context-bound 3
// RUN: --context-bound 3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
