int __VERIFIER_nondet_int();
int g(int d)
{
  if (d == 3)
    return __VERIFIER_nondet_int();
  int a = 0, b = 0;
  for (int i = 0; i < 2; i++)
  {
    int r = g(d + 1);
    if (i == 0)
      a = r;
    else
      b = r;
  }
  if (d == 2)
    __VERIFIER_assert(a == b); // two calls may return different values
  return a;
}
int main()
{
  return g(2);
}
// Imported from ESBMC regression/esbmc/loop_local_l1_reuse_cutoff_fail
// RUN: --unwind 1 --multi-property
// EXPECT: FAILED +\[g\.assertion\.2\]
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict FAILED
