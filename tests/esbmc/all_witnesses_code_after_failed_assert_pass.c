/* The twin of all_witnesses_code_after_failed_assert: the branch restores x,
   so w holds on every path and must be reported PASSED (esbmc/esbmc#7900). */
int main()
{
  int x = 0;
  if (__VERIFIER_nondet_int())
  {
    __VERIFIER_assert(0); // v
    x = 1;
    x = x - 1;
  }
  __VERIFIER_assert(x == 0); // w
  return 0;
}
// Imported from ESBMC regression/esbmc/all_witnesses_code_after_failed_assert_pass
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.\d+\]\s+line\s+12\s+w$
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+8\s+v$
// Vereda: verdict none
