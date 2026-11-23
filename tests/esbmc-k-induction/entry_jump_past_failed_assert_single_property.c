/* Safe: i == 0 whenever L is reached. Without --multi-property the entry jump
   past assert(0) must stay unhavoced, or this proof is lost (esbmc/esbmc#7900). */
int __VERIFIER_nondet_int();
int main()
{
  int i = 0, x = 0;
  if (__VERIFIER_nondet_int())
  {
    x = 1;
    goto L;
  }
  while (__VERIFIER_nondet_int())
  {
    i = 0;
    continue;
  L:
    if (i == 0)
      goto out;
    __VERIFIER_assert(0); // v
  }
out:
  return x;
}
// Imported from ESBMC regression/k-induction/entry_jump_past_failed_assert_single_property
// Needs: --k-induction
// RUN: --k-induction --max-k-step 5
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
