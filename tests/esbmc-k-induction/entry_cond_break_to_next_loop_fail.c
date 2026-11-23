int __VERIFIER_nondet_int();
int main()
{
  int k = __VERIFIER_nondet_int(), c = 0, i = 0, j = 0;
  if (k < 0 || k > 100)
    return 0;
  while (i++ < 1000)
  {
    if (!(c < k))
      break;
    c++;
  }
  while (j < 10)
    j++;
  __VERIFIER_assert(c < k); // c < k
}
// Imported from ESBMC regression/k-induction/entry_cond_break_to_next_loop_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 20
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
