// A symbolic size reaches the solver as a quantified definition of the
// whole array.
unsigned __VERIFIER_nondet_uint(void);
int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 77 && n < 100);
  int a[n];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;
  for (unsigned x = 0; x < n; x++)
    __VERIFIER_assert(a[x] != 77); // a[x] != 77
}
// Imported from ESBMC regression/esbmc/array_loop_summary_vla_fail
// Needs: --summarize-array-loops
// RUN: --summarize-array-loops --no-slice
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
