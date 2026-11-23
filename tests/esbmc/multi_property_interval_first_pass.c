/* esbmc/esbmc#7900: __VERIFIER_assert(0, ...) reaches the first interval pass as
   a literal ASSERT 0, and that pass treated it as the end of the path: x = 1
   was never seen, and w was checked as though x were always 0. */
int main()
{
  int x = 0;
  if (__VERIFIER_nondet_int())
  {
    __VERIFIER_assert(0); // v
    x = 1;
    x = x + 1;
  }
  __VERIFIER_assert(x == 0); // w
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_interval_first_pass
// Needs: --interval-analysis
// RUN: --interval-analysis --multi-property
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+13\s+w$
// Vereda: verdict none
