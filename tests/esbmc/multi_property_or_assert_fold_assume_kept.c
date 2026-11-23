/* The fold dropped the assume(0) after a failed assertion (esbmc/esbmc#7900). */
int main()
{
  int c = __VERIFIER_nondet_int();
  if (!c)
  {
    __VERIFIER_assert(0); // v
    __VERIFIER_assume(0);
  }
  __VERIFIER_assert(c != 0); // w
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_or_assert_fold_assume_kept
// RUN: --multi-property
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.\d+\]\s+line\s+10\s+w$
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+7\s+v$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.assertion\.\d+\]\s+line\s+10\s+w$
// MODE: FUTURE
// Vereda: verdict FAILED
