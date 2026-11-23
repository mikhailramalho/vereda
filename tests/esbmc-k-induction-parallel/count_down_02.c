int __VERIFIER_nondet_int();
int main() { 

  int fail=0;
  int x=__VERIFIER_nondet_int();
  __VERIFIER_assume(x>0);
  while(x>0) {
    __VERIFIER_assume(fail!=1);
    if(fail==1) {
      x= -1;
//      fail=2;
    } else {
      x--;
    }
  }
  assert(x==0);
}

// Imported from ESBMC regression/k-induction-parallel/count_down_02
// Needs: --k-induction-parallel --std=gnu89
// RUN: --k-induction-parallel --std=gnu89
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
