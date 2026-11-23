#define a (2)
int __VERIFIER_nondet_int();
int main() { 
  int i, n=__VERIFIER_nondet_int(), sn=0;
  for(i=1; i<=n-2; i++) {
    //if (i<10) //first case
    sn = sn + a; //second case OK
    //if (i==4) sn=-10; //third case OK
  }
  //__VERIFIER_assume(i>n);
  assert(sn==n*a || sn == 0);
}
// Imported from ESBMC regression/k-induction/sum05_bug
// Needs: --k-induction
// RUN: --k-induction  -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
