// Congruence must not over-constrain: an uninterpreted function is NOT forced
// to be constant. With distinct arguments it may return distinct results, so
// this assertion is genuinely violable and ESBMC must report it. This guards
// against an unsound "always returns the same value" simplification.
int __CPROVER_uninterpreted_f(int);

int main()
{
  int x, y;
  __VERIFIER_assume(x != y);
  int a = __CPROVER_uninterpreted_f(x);
  int b = __CPROVER_uninterpreted_f(y);
  __VERIFIER_assert(a == b); // distinct arguments need not give equal results
  return 0;
}
// Imported from ESBMC regression/esbmc/cprover_uninterpreted_distinct_fail
// Needs: __CPROVER_uninterpreted_f
// RUN: --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
