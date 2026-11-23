int __VERIFIER_nondet_int();
int main() { 

  int fail=0;
  int x=__VERIFIER_nondet_int();
  __VERIFIER_assume(x>0);
  while(x>0) {
    if(fail==1) {
      x= -1;
      fail=2;
    } else {
      x--;
    }
  }
  assert(x==0);
}

// Imported from ESBMC regression/k-induction-parallel/count_down
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
