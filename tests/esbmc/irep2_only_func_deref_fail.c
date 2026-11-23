/* The re-taken address designates the function it came from, so claiming a
   different one must be caught. */
int f(void)
{
  return 7;
}

int g(void)
{
  return 8;
}

int main(void)
{
  /* Repeated, so the arm has to fire more than once. */
  __VERIFIER_assert(***f == &g); // *f is f, not g
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_func_deref_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
