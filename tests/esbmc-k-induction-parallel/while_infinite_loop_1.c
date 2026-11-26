
int __VERIFIER_nondet_int();

int main() {
  int x=0;

  while(1)
  {
    assert(x==0);    
  }

  assert(x!=0);
}
// Imported from ESBMC regression/k-induction-parallel/while_infinite_loop_1
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel --std=gnu89
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
