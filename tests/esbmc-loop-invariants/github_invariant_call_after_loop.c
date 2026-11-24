/* An invariant that calls a function, on a loop preceded by another loop. The
 * pass moves the call's temporaries out of the body; erasing them dangled the
 * first loop's exit target and compute_target_numbers aborted. */
int g(int k)
{
  return k;
}

int main(void)
{
  int i, total = 0;

  for (i = 0; i < 2; i++)
    total += i;

  __ESBMC_loop_invariant(total == g(i));
  for (i = 0; i < 2; i++)
    total += i;

  __VERIFIER_assert(total >= 0); // reached a verdict rather than aborting
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_invariant_call_after_loop
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --unwind 4
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
