#define a (2)
int __VERIFIER_nondet_int();
int main()
{
  int i, n = __VERIFIER_nondet_int(), sn = 0;
  for (i = 1; i <= n; i++)
    sn = sn + a;

  assert(sn == n * a || sn == 0);
}
// Imported from ESBMC regression/k-induction-parallel/sum01
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
