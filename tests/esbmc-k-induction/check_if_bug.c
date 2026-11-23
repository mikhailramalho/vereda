int __VERIFIER_nondet_int();

int main() {

  int x=__VERIFIER_nondet_int();
  __VERIFIER_assume(x>0);
  int y=__VERIFIER_nondet_int();
  __VERIFIER_assume(y>=0);
  __VERIFIER_assume(y<1);
  int z=0;

  while(x>0) {
      x--;
      if(x==y) z=1; // assert(0);
  }
  assert(z==0);
}

// Imported from ESBMC regression/k-induction/check_if_bug
// Needs: --k-induction
// RUN: --k-induction -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
