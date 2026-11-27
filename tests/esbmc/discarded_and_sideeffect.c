/* The && arm: discarded `A && f()` is `if (A) f();`, so f runs iff A holds. */
int calls;
int bump(void)
{
  calls++;
  return 1;
}
int main(void)
{
  unsigned n;
  __VERIFIER_assume(n < 4);
  (void)((n > 2) && bump());
  __VERIFIER_assert(calls == (n > 2 ? 1 : 0)); // bump runs iff n > 2
  return 0;
}
// Imported from ESBMC regression/esbmc/discarded_and_sideeffect
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
