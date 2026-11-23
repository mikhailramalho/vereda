int __VERIFIER_nondet_int();

int main() {

  int x=__VERIFIER_nondet_int();
  __VERIFIER_assume(x>0);
  int y=__VERIFIER_nondet_int();
  __VERIFIER_assume(y>=0);
  __VERIFIER_assume(y<x);
  int z=0;

  while(x>0) {
      x--;
      if(x==y) z=1; // assert(0);
  }
  assert(z==1);
}

// Imported from ESBMC regression/k-induction-parallel/check_if
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
