
int __VERIFIER_nondet_int();

int main() {
  int i=0, x=0, y=0;
  int n=__VERIFIER_nondet_int();
  __VERIFIER_assume(n>0);
  for(i=0; 1; i++)
  {
    assert(x==0);
  }
  assert(x==0);
}

// Imported from ESBMC regression/k-induction-parallel/for_infinite_loop_1
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
