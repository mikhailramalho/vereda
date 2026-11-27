/* The (c) || (assert(0), 0) fold erased x = 1 (esbmc/esbmc#7900). */
int main()
{
  int x = 0;
  if (__VERIFIER_nondet_int())
  {
    __VERIFIER_assert(0); // v
    x = 1;
  }
  __VERIFIER_assert(x == 0); // w
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_or_assert_fold_keeps_code
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+10\s+w$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+10\s+w$
// MODE: FUTURE
// Vereda: verdict FAILED
