// Soundness probe (cf. the closed, unsound PR #5283). An uninterpreted
// comparator may legitimately return 0 for any pair of arguments, so asserting
// the result is non-zero must FAIL. Modelling these calls as "always non-zero"
// would silently mask real violations; this test pins that down.
int __CPROVER_uninterpreted_eq(int, int);

int main()
{
  int x;
  int r = __CPROVER_uninterpreted_eq(x, x);
  __VERIFIER_assert(r != 0); // uninterpreted function may return zero
  return 0;
}
// Imported from ESBMC regression/esbmc/cprover_uninterpreted_nonzero_fail
// Needs: __CPROVER_uninterpreted_eq
// RUN: --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
