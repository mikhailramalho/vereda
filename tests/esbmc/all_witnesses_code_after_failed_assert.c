/* esbmc/esbmc#7900: --all-witnesses keeps checking claims past a violation,
   but remove_unreachable still ran for it and deleted the code after the
   failed assertion, so w was reported PASSED. */
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
// Imported from ESBMC regression/esbmc/all_witnesses_code_after_failed_assert
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+13\s+w$
// Vereda: verdict none
