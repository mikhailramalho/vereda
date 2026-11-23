#define a (2)
int __VERIFIER_nondet_int();

int main()
{
  int i=1, n = __VERIFIER_nondet_int(), sn = 0;
  __VERIFIER_assume(n < 1000 && n >= -1000);

  while (i <= n)
  {
    sn = sn + a;
    i++;
    assert(sn == (i-1) * a);
  }

  assert(sn == n * a || sn == 0);
}
// Imported from ESBMC regression/k-induction-parallel/sum06
// Needs: --k-induction-parallel --std=gnu89
// RUN: --k-induction-parallel --std=gnu89
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
