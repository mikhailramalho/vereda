/* As multi_property_or_assert_fold_keeps_code, via the native converter
   (esbmc/esbmc#7900). */
int main()
{
  int c = __VERIFIER_nondet_int();
  int x = 0;
  if (c)
  {
    __VERIFIER_assert(0); // v
    x = 1;
  }
  __VERIFIER_assert(x == 0); // w
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_or_assert_fold_keeps_code_native
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+12\s+w$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+12\s+w$
// MODE: FUTURE
// Vereda: verdict FAILED
