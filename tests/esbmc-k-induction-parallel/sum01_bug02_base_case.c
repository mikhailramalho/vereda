#define a (2)
unsigned int __VERIFIER_nondet_uint();
int main() { 
  int i, n=__VERIFIER_nondet_uint(), sn=0;
  for(i=1; i<=n; i++) {
    //if (i<10) //first case
    sn = sn + a; //second case OK
    if (i==4) sn=-10; //third case OK
  }
  //__VERIFIER_assume(i>n);
  assert(sn==n*a || sn == 0);
}
// Imported from ESBMC regression/k-induction-parallel/sum01_bug02_base_case
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
