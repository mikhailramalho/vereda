extern void __VERIFIER_error(void) __attribute__((__noreturn__));
void __VERIFIER_assert(int cond)
{
  if (!(cond))
  {
  ERROR:
    __VERIFIER_error();
  }
}

int main(void)
{
  int n = 0;
  int v, x;
  for (x = 0; x < n; x++)
    __VERIFIER_assert(0);
  for (v = 0; v < 4; v++)
    n = n + 1;
  for (x = 0; x < n; x++)
    __VERIFIER_assert(x < 2);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_29_dead_loop_fail
// Needs: --k-induction --max-inductive-step 3 --interval-analysis
// RUN: -m32 --k-induction --max-inductive-step 3 --interval-analysis --error-label ERROR
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
