/* The dead entry jump now gets a havoc; the inductive-step proof must survive
   (esbmc/esbmc#7900). */
int __VERIFIER_nondet_int();
int main()
{
  int i = 0, x = 0, y = 0;
  if (x == 1)
  {
    y = 1;
    goto L;
  }
  while (i < 10)
  {
    __VERIFIER_assert(i < 10); // bound
    i = i + 1;
    continue;
  L:
    __VERIFIER_assert(0); // v
    i = i + 1;
  }
  return y;
}
// Imported from ESBMC regression/k-induction/multi_property_entry_jump_past_failed_assert_pass
// Needs: --k-induction
// RUN: --k-induction --multi-property
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.\d+\]\s+line\s+14\s+bound$
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
