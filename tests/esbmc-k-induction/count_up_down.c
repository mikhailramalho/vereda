unsigned int __VERIFIER_nondet_uint();

int main()
{
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned int x=n, y=0;
  while(x>0)
  {
    x--;
    y++;
  }
  assert(y==n);
}

// Imported from ESBMC regression/k-induction/count_up_down
// Needs: --k-induction
// RUN: --k-induction
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
