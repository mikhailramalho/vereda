
int __VERIFIER_nondet_int();

int main() {
  int x=0;

  while(1)
  {
    assert(x==0);    
  }

  assert(x!=0);
}
// Imported from ESBMC regression/k-induction/while_infinite_loop_1
// Needs: --k-induction --std=gnu89
// RUN: --k-induction --std=gnu89
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
