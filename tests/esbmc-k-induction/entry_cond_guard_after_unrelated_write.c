// i++ writes before the break, but not what c < k reads, so the inductive step
// may still assume c < k at the head.
int main()
{
  int k = __VERIFIER_nondet_int();
  if (k < 0 || k > 100)
    return 0;
  int c = 0, i = 0;
  while (i++ < 1000)
  {
    if (!(c < k))
      break;
    c++;
  }
  __VERIFIER_assert(c <= k); // c <= k
}
// Imported from ESBMC regression/k-induction/entry_cond_guard_after_unrelated_write
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
