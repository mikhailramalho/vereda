#include<assert.h>

int main(){
 int x=1, y=0;

 while(y <1000 &&__VERIFIER_nondet_int())
 {
   x=x+y;
   y=y+1;
 }

 assert(x >= y);

 return 0;
}

// Imported from ESBMC regression/goto-contractor/github_2662
// Needs: --interval-analysis --goto-contractor
// RUN: --interval-analysis --goto-contractor --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
