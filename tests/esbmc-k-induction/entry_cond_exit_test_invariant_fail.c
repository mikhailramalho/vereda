// *p reads x, and x += 101 after the exit test changes it, so *p < 100 does
// not hold at the next head and must not be assumed there.
int __VERIFIER_nondet_int();
int main()
{
  int x = 90, m = 0;
  int *p = &x;
  while (1)
  {
    __VERIFIER_assert(x != 105); // x != 105
    x -= 100;
    m = __VERIFIER_nondet_int();
    if (*p >= 100 || m == 12345)
      break;
    x += 101;
  }
}
// Imported from ESBMC regression/k-induction/entry_cond_exit_test_invariant_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 40
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
