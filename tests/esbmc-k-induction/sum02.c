unsigned int __VERIFIER_nondet_uint();

int main()
{
  unsigned int i, n = __VERIFIER_nondet_uint(), sn = 0;
  for (i = 0; i <= n; i++)
    sn = sn + i;

  assert(sn == (n * (n + 1)) / 2 || sn == 0);
}
// Imported from ESBMC regression/k-induction/sum02
// Needs: --k-induction
// RUN: --k-induction
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
