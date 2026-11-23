// The inner loop's exit test reads j after j++. The step may assume it only
// for a state before the loop that passes it: when m <= 3 the inner loop
// leaves in its first iteration, and n still reaches 20.
int __VERIFIER_nondet_int();
int main()
{
  int m = __VERIFIER_nondet_int();
  int n = 0;
  while (1)
  {
    n++;
    int j = 0;
    while (1)
    {
      j++;
      if (!(m > 3 && j < 10))
        break;
    }
    __VERIFIER_assert(!(m <= 3 && n == 20)); // n != 20
  }
}
// Imported from ESBMC regression/k-induction/entry_cond_guard_after_unrelated_write_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 30
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
