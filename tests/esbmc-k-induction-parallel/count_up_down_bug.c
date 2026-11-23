unsigned int __VERIFIER_nondet_uint();

int main()
{
  unsigned int n = __VERIFIER_nondet_uint();
//  __VERIFIER_assume(n>0 && n<10000);
  unsigned int x=n, y=0;
//  __VERIFIER_assume(x==n);
  while(x>0)
  {
    x--;
    y++;
  }
  assert(y!=n);
//  assert(x==0);
}
// Imported from ESBMC regression/k-induction-parallel/count_up_down_bug
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
